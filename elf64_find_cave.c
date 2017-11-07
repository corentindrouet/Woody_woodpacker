#include "woody_woodpacker.h"

int		elf64_find_cave(void *f_map, size_t f_size, size_t ps_size, off_t *c_offset, size_t *c_size)
{
	char		*buf;
	off_t		start;
	off_t		end;
	off_t		off;
	off_t		max;

	buf = (char *)f_map;
	start = 0;
	end = 0;
	off = 0;
	max = 0;
	while (((size_t)start < f_size))
	{
		if (end != 0 && (size_t)max >= ps_size + 256)
		{
			*c_offset = off;
			*c_size = (size_t)max;
			return (0);
		}
		if (buf[start] != 0)
			start++;
		else
		{
			end = start;
			while ((size_t)end < f_size && buf[end] == 0)
				end++;
			if ((end - start) > max)
			{
				off = start;
				max = end - start;
			}
			start = end + 1;
		}
	}
	return (-1);
}
