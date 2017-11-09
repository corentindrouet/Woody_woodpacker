#include "woody_woodpacker.h"

int		file_map(const char *name, void **f_map, size_t *f_size, int prot)
{
	int			fd;

	if (!name)
		return (-1);
	fd = open(name, (prot == 0) ? O_RDONLY : O_RDWR);
	if (fd < 0)
		return (-1);
	*f_size = file_size(fd);
	*f_map = mmap(0, *f_size, (prot == 0) ? PROT_READ : PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (*f_map == MAP_FAILED)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}
