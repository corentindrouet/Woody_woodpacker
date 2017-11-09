#include "woody_woodpacker.h"

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
			printf("[+] Section .text of %s is at file offset %lu (%#lx bytes)\n", TARGET_FILE, stable.sh_offset, stable.sh_size);
			return (0);
		}
		i++;
	}
	return (-1);
}
