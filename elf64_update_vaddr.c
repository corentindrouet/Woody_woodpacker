#include "woody_woodpacker.h"

int		elf64_update_vaddr(void *f_map, size_t ps_size)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*c_phdr;
	Elf64_Phdr	*n_phdr;
	int			i;

	ehdr = (Elf64_Ehdr *)f_map;
	i = 0;
	while (i < ehdr->e_phnum)
	{
		c_phdr = (Elf64_Phdr *)(f_map + ehdr->e_phoff + (i * sizeof(Elf64_Phdr)));
		if (c_phdr->p_type == PT_LOAD && (c_phdr->p_flags & (PF_R | PF_X)))
		{
			if ((i + 1) < ehdr->e_phnum)
			{
				// Next segment
				n_phdr = (Elf64_Phdr *)(f_map + ehdr->e_phoff + ((i + 1) * sizeof(Elf64_Phdr)));
				if ((n_phdr->p_offset - (c_phdr->p_offset + c_phdr->p_filesz)) > ps_size)
				{
                    c_phdr->p_filesz += ps_size;
                    c_phdr->p_memsz += ps_size;
					return (0);
				}
				else
					return (-1);
			}
		}
		i++;
	}
	return (-1);
}
