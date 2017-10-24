#include "woody_woodpacker.h"

size_t		get_file_size(int fd)
{
	size_t			size;
	
	lseek(fd, 0, SEEK_SET);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return (size);
}
