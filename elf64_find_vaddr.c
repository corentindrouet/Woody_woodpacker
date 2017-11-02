#include "woody_woodpacker.h"

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
			return (0);
		}
		i++;
	}
	return (-1);
}
