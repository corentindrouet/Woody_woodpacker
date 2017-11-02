#include "woody_woodpacker.h"

int		file_unmap(void *f_map, size_t f_size)
{
	if (!f_map)
		return (-1);
	return (munmap(f_map, f_size));
}
