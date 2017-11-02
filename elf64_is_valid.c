#include "woody_woodpacker.h"

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
