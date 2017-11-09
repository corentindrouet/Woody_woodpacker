#include "woody_woodpacker.h"

static int	elf64_find_cave_loop(void *f_map, size_t f_size, size_t ps_size, off_t *c_offset, size_t *c_size)
{
	char		*buf;
	off_t		start;
	off_t		end;

	buf = (char *)f_map;
	start = 0;
	end = 0;
	while (((size_t)start < f_size))
	{
		if (buf[start] != 0)
			start++;
		else
		{
			end = start;
			while ((size_t)end < f_size && buf[end] == 0)
				end++;
			if (end != 0 && (size_t)(end - start) >= (ps_size + 256))
			{
				// printf("%-15s %ld\n%-15s %lu\n\n", "cave offset", start, "cave size", end - start);
				*c_offset = start;
				*c_size = (size_t)(end - start);
				return (0);
			}
			start = end + 1;
		}
	}
	return (-1);
}

int		elf64_find_cave(void *f_map, size_t f_size, size_t ps_size, off_t *c_offset, size_t *c_size)
{
	Elf64_Ehdr		*ehdr;
	Elf64_Shdr		*shdr;
	Elf64_Shdr		stable;
	// char			*sname;
	int				i;
	int				ret;

	(void)f_size;
	(void)c_offset;
	(void)c_size;

	ehdr = (Elf64_Ehdr *)f_map;
	shdr = (Elf64_Shdr *)(f_map + ehdr->e_shoff);
	stable = shdr[ehdr->e_shstrndx];
	// sname = (char *)(f_map + stable.sh_offset);

	ret = -1;
	i = 0;
	while (i < ehdr->e_shnum)
	{
		stable = shdr[i];
		if (elf64_find_cave_loop(f_map + stable.sh_offset, stable.sh_offset, ps_size, c_offset, c_size) != -1)
		{
			// printf("[*] Section %s\n", sname + stable.sh_name);
			// printf("%-15s %lu\n", "sect flags", stable.sh_flags);
			// printf("%-15s %lu\n", "sect offset", stable.sh_offset);
			// printf("%-15s %lu\n", "sect size", stable.sh_size);
			// printf("%-15s %lu\n", "sect addr", stable.sh_addr);
		}
		i++;
	}
	return (ret);
}
