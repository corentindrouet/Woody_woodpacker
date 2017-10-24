#include "woody_woodpacker.h"

int			check_elf(const char *src)
{
	int		fd;
	char	*buf;

	// Open file
	if ((fd = open(src, O_RDONLY)) < 0)
		return (-1);
	
	// Create buffer
	if ((buf = (char *)malloc(sizeof(char) * 5)) == NULL)
	{
		close(fd);
		return (-1);
	}
	
	// Place file cursor at 0
	lseek(fd, 0, SEEK_SET);
	
	// Get the first 5 bytes
	if (read(fd, buf, 5) <= 0)
	{
		close(fd);
		free(buf);
		return (-1);
	}
	
	// Check if "ELF" magic number
	if (buf[0] != 0x7f || buf[1] != 'E' || buf[2] != 'L' || buf[3] != 'F')
	{
		close(fd);
		free(buf);
		return (-1);
	}

	// Check if 64 bit architecture
	if (buf[4] != 2)
	{
		close(fd);
		free(buf);
		return (-1);
	}

	// Close, free, return
	close(fd);
	free(buf);
	return (0);
}
