#include "woody_woodpacker.h"

void	munmap_file(t_file *file)
{
	if (file->fd > 0)
	{
		munmap(file->map, file->size);
		close(file->fd);
	}
}

int		mmap_file(const char *name, t_file *file)
{
	struct stat		fst;

	file->fd = open(name, O_RDWR);
	if (file->fd < 0)
		return (-1);
	if (fstat(file->fd, &fst) == -1)
	{
		close(file->fd);
		return (-1);
	}
	file->size = fst.st_size;
	if ((file->map = mmap(NULL, file->size, PROT_READ|PROT_WRITE, MAP_PRIVATE, file->fd, 0)) == MAP_FAILED)
	{
		close(file->fd);
		return (-1);
	}
	return (0);
}
