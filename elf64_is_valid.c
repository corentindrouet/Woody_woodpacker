#include "woody_woodpacker.h"

int		elf64_is_valid(Elf64_Ehdr *ehdr)
{
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || \
			ehdr->e_ident[EI_MAG1] != ELFMAG1 || \
			ehdr->e_ident[EI_MAG2] != ELFMAG2 || \
			ehdr->e_ident[EI_MAG3] != ELFMAG3)
	{
		return (-1);
	}
	return (0);
}

int		elf64_is_rel(Elf64_Ehdr *ehdr)
{
	if (elf64_is_valid(ehdr) == 0 && ehdr->e_type == ET_REL)
		return (0);
	return (-1);
}

int		elf64_is_exec(Elf64_Ehdr *ehdr)
{
	if (elf64_is_valid(ehdr) == 0 && ehdr->e_type == ET_EXEC)
		return (0);
	return (-1);
}

int		elf64_is_dyn(Elf64_Ehdr *ehdr)
{
	if (elf64_is_valid(ehdr) == 0 && ehdr->e_type == ET_DYN)
		return (0);
	return (-1);
}
