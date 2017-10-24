#include "woody_woodpacker.h"

int					packer_infect(int fd, char *packer)
{
	unsigned int	offset;

	// Reset fd cursor
	lseek(fd, 0, SEEK_SET);
	
	// Code cave offset
	offset = cave_miner(fd, strlen(packer));
	if (offset == 0)
		return (-1);

	// Jump to code cave offset
	lseek(fd, offset, SEEK_SET);

	// Write packer
	if (write(fd, packer, strlen(packer)) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: write failed\n");
		return (-1);
	}
	else
	{
		printf("Success: packer wrote into file\n");
	}

	// Reset fd cursor
	lseek(fd, 0, SEEK_SET);
	return (0);
}
