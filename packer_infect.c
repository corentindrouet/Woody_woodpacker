#include "woody_woodpacker.h"

/*
**	Create a function here to update packer code with
**		>> previous Elf64_Ehdr->e_entry for final packer jump
**		>> .text section address
**		>> .text section size
*/

unsigned int		packer_infect(int fd, char *packer)
{
	unsigned int	offset;

	// Reset fd cursor
	lseek(fd, 0, SEEK_SET);
	
	// Code cave offset
	offset = cave_miner(fd, strlen(packer));
	if (offset == 0)
		return (0);

	// Jump to code cave offset
	lseek(fd, offset, SEEK_SET);

	// Write packer
	if (write(fd, packer, strlen(packer)) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		return (0);
	}

	// Reset fd cursor
	lseek(fd, 0, SEEK_SET);
	return (offset);
}
