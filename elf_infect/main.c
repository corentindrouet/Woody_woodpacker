#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

#define PAGE_SIZE	4096

typedef struct		s_file
{
	int				fd;
	void			*map;
	off_t			size;
	Elf64_Off		sh_offset;
	uint64_t		sh_size;
}					t_file;

void	*get_random_key(size_t size)
{
	void	*buffer;
	int		fd;
	int		numberRandomBytesReaded;

	numberRandomBytesReaded = 0;
	if ((fd = open("/dev/urandom", O_RDONLY)) == -1)
		return (NULL);
	if (!(buffer = malloc(size + 1)))
		return (NULL);
	bzero(buffer, size + 1);
	while (numberRandomBytesReaded < 256)
	{
		read(fd, (buffer + numberRandomBytesReaded), size - numberRandomBytesReaded);
		numberRandomBytesReaded = strlen(buffer);
	}
	close(fd);
	return (buffer);
}

void	swap(int *a, int *b)
{
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
}

unsigned char	*encrypt_zone(char *zone, size_t size)
{
	unsigned char	*key;
	int				tab[256];
	int				i;
	int				j;
	size_t			k;

	if (!zone || !size || !(key = get_random_key(256)))
		return (0);
	i = -1;
	printf("Encryption key :\n%s\n", key);
	while (++i < 256)
		tab[i] = i;
	i = -1;
	j = 0;
	while (++i < 256)
	{
		j = (j + tab[i] + key[i % 256]) % 256;
		swap(&(tab[i]), &(tab[j]));
	}
	i = 0;
	j = 0;
	k = 0;
	while (k < size)
	{
		i = (i + 1) % 256;
		j = (j + tab[i]) % 256;
		swap(&(tab[i]), &(tab[j]));
		j = (tab[i] + tab[j]) % 256;
		zone[k] = zone[k] ^ tab[j];
		k++;
	}
	return (key);
}

int		patch_addr(void *m, size_t len, uint64_t pat, uint64_t val)
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
		return (-1);
	if (fstat(file->fd, &fst) == -1)
	{
		close(file->fd);
		return (-1);
	}
	file->size = fst.st_size;
	if ((file->map = mmap(NULL, file->size, PROT_READ|PROT_WRITE, MAP_PRIVATE, file->fd, 0)) == MAP_FAILED)
	{
		close(file->fd);
		return (-1);
	}
	return (0);
}

int		find_elf64_section(t_file *file, const char *sect)
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
	for (shnum = 0 ; shnum < ehdr->e_shnum ; shnum++)
	{
		stable = shdr[shnum];
		if (!strcmp(sname + stable.sh_name, sect))
		{
			file->sh_offset = stable.sh_offset;
			file->sh_size = stable.sh_size;
			return (0);
		}
	}
	return (-1);
}

int		check_elf64_header(Elf64_Ehdr *ehdr)
{
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || \
			ehdr->e_ident[EI_MAG1] != ELFMAG1 || \
			ehdr->e_ident[EI_MAG2] != ELFMAG2 || \
			ehdr->e_ident[EI_MAG3] != ELFMAG3 || \
			ehdr->e_ident[EI_CLASS] != ELFCLASS64)
	{
		return (-1);
	}
	return (0);
}

int		main(int ac, char **av)
{
	t_file			host;
	t_file			virus;
	int				woodyfd;
	Elf64_Ehdr		*ehdr;
	Elf64_Phdr		*phdr;
	Elf64_Shdr		*shdr;
	uint16_t		phnum;
	uint16_t		shnum;
	int				text_found;
	int				virus_offset;
	int				base_address;
	int				old_entry;
	int				pad;
	int				i;
	unsigned char	*key;

	if (ac != 3)
	{
		printf("Usage: ./%s <host> <virus>\n", av[0]);
		return (-1);
	}

	if (mmap_file(av[1], &host) == -1 || mmap_file(av[2], &virus) == -1)
	{
		printf("Error: Unable to open target file(s)\n");
		munmap_file(&host);
		munmap_file(&virus);
		return (-1);
	}

	if (check_elf64_header((Elf64_Ehdr *)(host.map)) == -1 || check_elf64_header((Elf64_Ehdr *)(virus.map)) == -1)
	{
		printf("Error: Invalid ELF header(s)\n");
		munmap_file(&host);
		munmap_file(&virus);
		return (-1);
	}

	if (((Elf64_Ehdr *)(host.map))->e_type != ET_EXEC)
	{
		printf("Error: Host file is not an ET_EXEC\n");
		munmap_file(&host);
		munmap_file(&virus);
		return (-1);
	}

	if (find_elf64_section(&host, ".text") == -1 || find_elf64_section(&virus, ".text") == -1)
	{
		printf("Error: Unable to find .text section(s)\n");
		munmap_file(&host);
		munmap_file(&virus);
		return (-1);
	}

	ehdr = (Elf64_Ehdr *)(host.map);
	phdr = (Elf64_Phdr *)(host.map + ehdr->e_phoff);
	shdr = (Elf64_Shdr *)(host.map + ehdr->e_shoff);
	text_found = 0;
	virus_offset = 0;
	old_entry = 0;
	i = 0;
	pad = 0;

	ehdr->e_shoff += PAGE_SIZE;

	for (phnum = 0 ; phnum < ehdr->e_phnum ; phnum++, phdr++)
	{
		if (text_found && phdr->p_offset >= virus_offset)
			phdr->p_offset += PAGE_SIZE;
		else if (phdr->p_type == PT_LOAD && phdr->p_flags & PF_X)
		{
			virus_offset = phdr->p_offset + phdr->p_filesz;
			old_entry = ehdr->e_entry;
			base_address = phdr->p_vaddr;
			text_found++;

			ehdr->e_entry = phdr->p_vaddr + phdr->p_filesz + 256;
			phdr->p_filesz += virus.sh_size + 256;
			phdr->p_memsz += virus.sh_size + 256;
		}
	}

	for (shnum = 0 ; shnum < ehdr->e_shnum ; shnum++, shdr++)
	{
		if (shdr->sh_offset + shdr->sh_size == virus_offset)
			shdr->sh_size += virus.sh_size + 256;
		if (shdr->sh_offset >= virus_offset)
			shdr->sh_offset += PAGE_SIZE;
	}

	if ((woodyfd = open("woody", O_TRUNC|O_CREAT|O_WRONLY, S_IRUSR|S_IXUSR|S_IWUSR)) < 0)
	{
		printf("Error: Unable to create woody\n");
		munmap_file(&host);
		munmap_file(&virus);
		return (-1);
	}

	key = encrypt_zone((char *)(host.map + host.sh_offset), host.sh_size);

	uint64_t tmp_p = base_address + host.sh_offset;
	uint64_t tmp_l = host.sh_offset + host.sh_size;

	patch_addr(virus.map + virus.sh_offset, virus.sh_size, 0x1111111111111111, (uint64_t)(base_address + host.sh_offset));
	patch_addr(virus.map + virus.sh_offset, virus.sh_size, 0x2222222222222222, (uint64_t)(host.sh_size));
	patch_addr(virus.map + virus.sh_offset, virus.sh_size, 0x3333333333333333, (uint64_t)(ehdr->e_entry - 256));
	patch_addr(virus.map + virus.sh_offset, virus.sh_size, 0x4444444444444444, (uint64_t)(old_entry));
	patch_addr(virus.map + virus.sh_offset, virus.sh_size, 0x5555555555555555, tmp_p - (tmp_p % 4096));
	patch_addr(virus.map + virus.sh_offset, virus.sh_size, 0x6666666666666666, tmp_l);

	write(woodyfd, host.map, virus_offset);

	write(woodyfd, key, 256);

	write(woodyfd, virus.map + virus.sh_offset, virus.sh_size);

	for (i = 0 ; i < PAGE_SIZE - virus.sh_size - 256 ; i++)
	{
		write(woodyfd, &pad, 1);
	}

	write(woodyfd, host.map + virus_offset, host.size - virus_offset);

	close(woodyfd);

	munmap_file(&host);
	munmap_file(&virus);
	return (0);
}
