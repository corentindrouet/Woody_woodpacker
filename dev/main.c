#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

typedef struct	s_datas
{
	void		*f_map;		/* Binary mmap pointer */
	size_t		f_size;		/* Binary file size */
	void		*p_map;		/* Packer mmap pointer */
	size_t		p_size;		/* Packer file size */
	uint64_t	o_entry;	/* Old entry point */
	uint64_t	n_entry;	/* New entry point */
	uint64_t	v_addr;		/* Virtual base address */
	off_t		fs_offset;	/* Binary section offset */
	size_t		fs_size;	/* Binary section size */
	off_t		ps_offset;	/* Packer section offset */
	size_t		ps_size;	/* Packer section size */
	off_t		c_offset;	/* Cave file offset */
	size_t		c_size;		/* Cave size */
}				t_datas;

int		elf64_update_asm(void *m, size_t len, uint32_t pat, uint32_t val)
{
	unsigned char	*p;
	uint64_t		n;
	int				i;

	p = (unsigned char *)m;
	i = 0;
	while (i < len)
	{
		n = *((uint32_t *)(p + i));
		if (n == pat)
		{
			*((uint32_t *)(p + i)) = val;
			return (0);
		}
		i++;
	}
	return (-1);
}

int		elf64_find_cave(void *f_map, size_t f_size, size_t ps_size, off_t *c_offset, size_t *c_size)
{
	char		*buf;
	off_t		start;
	off_t		end;
	off_t		off;
	off_t		max;

	buf = (char *)f_map;
	start = 0;
	end = 0;
	off = 0;
	max = 0;
	while ((start != f_size))
	{
		if (end != 0 && max >= ps_size)
		{
			*c_offset = off;
			*c_size = (size_t)max;
			return (0);
		}
		if (buf[start] != 0)
			start++;
		else
		{
			end = start;
			while (end != f_size && buf[end] == 0)
				end++;
			if ((end - start) > max)
			{
				off = start;
				max = end - start;
			}
			start = end + 1;
		}
	}
	return (-1);
}

int		elf64_find_sect(void *f_map, off_t *s_offset, size_t *s_size, const char *sect)
{
	Elf64_Ehdr		*ehdr;
	Elf64_Shdr		*shdr;
	Elf64_Shdr		stable;
	char			*sname;
	int				i;

	ehdr = (Elf64_Ehdr *)f_map;
	shdr = (Elf64_Shdr *)(f_map + ehdr->e_shoff);
	stable = shdr[ehdr->e_shstrndx];
	sname = (char *)(f_map + stable.sh_offset);

	i = 0;
	while (i < ehdr->e_shnum)
	{
		stable = shdr[i];
		if (!strcmp(sname + stable.sh_name, sect))
		{
			*s_offset = stable.sh_offset;
			*s_size = stable.sh_size;
			return (0);
		}
		i++;
	}
	return (-1);
}

int		elf64_find_vaddr(void *f_map, uint64_t *v_addr)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*phdr;
	int			i;

	ehdr = (Elf64_Ehdr *)f_map;
	i = 0;
	while (i < ehdr->e_phnum)
	{
		phdr = (Elf64_Phdr	*)(f_map + ehdr->e_phoff + (i * sizeof(Elf64_Phdr)));
		if (phdr->p_type == PT_LOAD && phdr->p_flags & 0x011)
		{
			*v_addr = phdr->p_vaddr;
			printf("Phdr: p_filesz=%lu p_memsz=%lu\n", phdr->p_filesz, phdr->p_memsz);
			return (0);
		}
		i++;
	}
	return (-1);
}

int		elf64_is_valid(Elf64_Ehdr *ehdr)
{
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 || ehdr->e_ident[EI_MAG2] != ELFMAG2 || ehdr->e_ident[EI_MAG3] != ELFMAG3 || ehdr->e_type != ET_EXEC)
	{
		return (-1);
	}
	return (0);
}

int		file_unmap(void *f_map, size_t f_size)
{
	if (!f_map)
		return (-1);
	return (munmap(f_map, f_size));
}

int		file_map(const char *name, void **f_map, size_t *f_size)
{
	int			fd;
	struct stat	st;

	if (!name)
		return (-1);
	fd = open(name, O_RDWR);
	if (fd < 0)
		return (-1);
	if (fstat(fd, &st) == -1)
	{
		close(fd);
		return (-1);
	}
	*f_size = st.st_size;
	*f_map = mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (*f_map == MAP_FAILED)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int		main(int argc, char **argv)
{
	t_datas		datas;

	datas.f_map = NULL;
	datas.f_size = 0x0;
	datas.p_map = NULL;
	datas.p_size = 0x0;
	datas.o_entry = 0x0;
	datas.n_entry = 0x0;
	datas.v_addr = 0x0;
	datas.fs_offset = 0x0;
	datas.fs_size = 0x0;
	datas.ps_offset = 0x0;
	datas.ps_size = 0x0;
	datas.c_offset = 0x0;
	datas.c_size = 0x0;

	if (argc != 3)
	{
		printf("Usage: %s <binary> <packer>\n", argv[0]);
		return (-1);
	}

	if (file_map(argv[1], &(datas.f_map), &(datas.f_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", argv[1]);
		return (-1);
	}

	if (file_map(argv[2], &(datas.p_map), &(datas.p_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", argv[1]);
		return (-1);
	}

	if (elf64_is_valid((Elf64_Ehdr *)datas.f_map) == -1)
	{
		printf("Error: %s ELF header is not valid\n", argv[1]);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	if (elf64_is_valid((Elf64_Ehdr *)datas.p_map) == -1)
	{
		printf("Error: %s ELF header is not valid\n", argv[2]);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	datas.o_entry = ((Elf64_Ehdr *)(datas.f_map))->e_entry;

	if (elf64_find_vaddr(datas.f_map, &(datas.v_addr)) == 0)
		printf("Virtual memory address : 0x%lx\n", datas.v_addr);

	if (elf64_find_sect(datas.f_map, &(datas.fs_offset), &(datas.fs_size), ".text") != -1)
		printf("Section .text of %s : file offset is %lu (%lu bytes)\n", argv[1], datas.fs_offset, datas.fs_size);

	if (elf64_find_sect(datas.p_map, &(datas.ps_offset), &(datas.ps_size), ".text") != -1)
		printf("Section .text of %s : file offset is %lu (%lu bytes)\n", argv[2], datas.ps_offset, datas.ps_size);

	if (elf64_find_cave(datas.f_map, datas.f_size, datas.ps_size, &(datas.c_offset), &(datas.c_size)) != -1)
		printf("Code cave in %s : file offset is %lu (%lu bytes)\n", argv[1], datas.c_offset, datas.c_size);

	datas.n_entry = datas.v_addr + datas.c_offset;

	printf("Old entry point address : 0x%lu\n", datas.o_entry);
	printf("New entry point address : 0x%lu\n", datas.n_entry);

	// Inject new content
	memmove(datas.f_map + datas.c_offset, datas.p_map + datas.ps_offset, datas.ps_size);

	// Update return address
	elf64_update_asm(datas.f_map + datas.c_offset, datas.ps_size, 0x22222222, (uint32_t)datas.o_entry);

	// Change entry point
	((Elf64_Ehdr *)(datas.f_map))->e_entry = datas.n_entry;

	// Unmap and save changes
	file_unmap(datas.f_map, datas.f_size);
	file_unmap(datas.p_map, datas.p_size);

	return (0);
}
