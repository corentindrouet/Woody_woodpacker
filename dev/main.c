#include "woody_woodpacker.h"

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
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || \
	ehdr->e_ident[EI_MAG1] != ELFMAG1 || \
	ehdr->e_ident[EI_MAG2] != ELFMAG2 || \
	ehdr->e_ident[EI_MAG3] != ELFMAG3 || \
	(ehdr->e_type != ET_DYN && ehdr->e_type != ET_EXEC))
	{
		return (-1);
	}
	return (0);
}

size_t	file_size(int fd)
{
	off_t	off;

	if (fd < 0)
		return (0);
	lseek(fd, 0, SEEK_SET);
	off = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	if (off == -1)
		return (0);
	return ((size_t)off);
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

	if (!name)
		return (-1);
	fd = open(name, O_RDWR);
	if (fd < 0)
		return (-1);
	*f_size = file_size(fd);
	*f_map = mmap(0, *f_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (*f_map == MAP_FAILED)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int		file_copy_free(int fd_src, int fd_dst, char **buf)
{
	if (*buf != NULL)
	{
		free(*buf);
		*buf = NULL;
	}
	close(fd_src);
	close(fd_dst);
	return (-1);
}

int		file_copy(const char *src, const char *dst)
{
	int		fd_src;
	int		fd_dst;
	size_t	size;
	ssize_t	ret;
	char	*buf;

	buf = NULL;
	fd_src = open(src, O_RDONLY);
	fd_dst = open(dst, O_RDWR | O_TRUNC | O_CREAT);
	if (fd_src < 0 || fd_dst < 0)
		return (file_copy_free(fd_src, fd_dst, &buf));
	size = file_size(fd_src);
	if (!(buf = (char *)malloc(sizeof(char) * size)))
		return (file_copy_free(fd_src, fd_dst, &buf));
	ret = read(fd_src, buf, size);
	if (ret < 0 || (size_t)ret != size)
		return (file_copy_free(fd_src, fd_dst, &buf));
	ret = write(fd_dst, buf, size);
	if (ret < 0 || (size_t)ret != size)
		return (file_copy_free(fd_src, fd_dst, &buf));
	file_copy_free(fd_src, fd_dst, &buf);
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

	if (argc != 2)
	{
		printf("Usage: %s <binary>\n", argv[0]);
		return (-1);
	}

	if (file_copy(argv[1], TARGET_FILE) == -1)
	{
		printf("Error: unable to copy binary file\n");
		return (-1);
	}
	else
	{
		printf("Debug: %s copied to %s\n", argv[1], TARGET_FILE);
	}

	if (file_map(TARGET_FILE, &(datas.f_map), &(datas.f_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", TARGET_FILE);
		return (-1);
	}
	else
	{
		printf("Debug: target '%s' mmaped\n", TARGET_FILE);
	}

	if (file_map(PACKER_FILE, &(datas.p_map), &(datas.p_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", PACKER_FILE);
		return (-1);
	}
	else
	{
		printf("Debug: packer '%s' mmaped\n", PACKER_FILE);
	}

	if (elf64_is_valid((Elf64_Ehdr *)datas.f_map) == -1)
	{
		printf("Error: target '%s' ELF header is not valid\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}
	else
	{
		printf("Debug: target '%s' has a valid ELF header\n", TARGET_FILE);
	}

	if (elf64_is_valid((Elf64_Ehdr *)datas.p_map) == -1)
	{
		printf("Error: %s ELF header is not valid\n", PACKER_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}
	else
	{
		printf("Debug: packer '%s' has a valid ELF header\n", PACKER_FILE);
	}

	datas.o_entry = ((Elf64_Ehdr *)(datas.f_map))->e_entry;

	if (elf64_find_vaddr(datas.f_map, &(datas.v_addr)) == 0)
		printf("Virtual memory address : 0x%lx\n", datas.v_addr);

	if (elf64_find_sect(datas.f_map, &(datas.fs_offset), &(datas.fs_size), ".text") != -1)
		printf("Section .text of %s : file offset is %lu (%lu bytes)\n", TARGET_FILE, datas.fs_offset, datas.fs_size);

	if (elf64_find_sect(datas.p_map, &(datas.ps_offset), &(datas.ps_size), ".text") != -1)
		printf("Section .text of %s : file offset is %lu (%lu bytes)\n", PACKER_FILE, datas.ps_offset, datas.ps_size);

	if (elf64_find_cave(datas.f_map, datas.f_size, datas.ps_size, &(datas.c_offset), &(datas.c_size)) != -1)
		printf("Code cave in %s : file offset is %lu (%lu bytes)\n", TARGET_FILE, datas.c_offset, datas.c_size);

	datas.n_entry = datas.v_addr + datas.c_offset;

	printf("Old entry point address : 0x%lu\n", datas.o_entry);
	printf("Old entry point address (diff) : 0x%lu\n", datas.v_addr + datas.fs_offset);
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
