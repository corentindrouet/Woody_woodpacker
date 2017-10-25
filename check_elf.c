#include "woody_woodpacker.h"

int			check_elf(int fd)
{
	char	*buf;

	if ((buf = (char *)malloc(sizeof(char) * 5)) == NULL)
		return (error_handler(E_ELFHDR));

	lseek(fd, 0, SEEK_SET);
	
	if (read(fd, buf, 5) <= 0)
	{
		free(buf);
		return (error_handler(E_ELFHDR));
	}
	
	lseek(fd, 0, SEEK_SET);
	
	if (buf[0] != 0x7f || buf[1] != 'E' || buf[2] != 'L' || buf[3] != 'F')
	{
		free(buf);
		return (error_handler(E_ELFHDR));
	}

	if (buf[4] != 2)
	{
		free(buf);
		return (error_handler(E_ELFHDR));
	}

	free(buf);
	return (0);
}
