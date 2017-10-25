#include "woody_woodpacker.h"

int		mine_cave(t_infos *infos)
{
	char		*buf;
	size_t		start;
	size_t		end;
	size_t		off;
	size_t		max;

	start = 0;
	end = 0;
	off = 0;
	max = 0;

	lseek(infos->dst_fd, 0, SEEK_SET);

	if ((buf = (char *)malloc(sizeof(char) * infos->file_size)) == NULL)
		return (error_handler(E_MCAVE));
	else
		printf(">> Debug: buffer malloc-ed for %ld bytes\n", infos->file_size);
	
	if (read(infos->dst_fd, buf, infos->file_size) != infos->file_size)
		return (error_handler(E_MCAVE));
	else
		printf(">> Debug: all file temporarily copied to buf\n");

	while ((start != (size_t)infos->file_size))
	{
		if (end != 0 && max >= strlen(infos->packer_code))
		{
			infos->cave_offset = off;
			printf(">> Debug: cave offset found at %ld with %ld bytes available\n", off, max);
			break;
		}

		if (buf[start] != 0)
		{
			start++;
		}
		else
		{
			end = start;
			while (end != (size_t)infos->file_size && buf[end] == 0)
			{
				end++;
			}
			
			if ((end - start) > max)
			{
				off = start;
				max = end - start;
			}
			
			start = end + 1;
		}
	}
	
	lseek(infos->dst_fd, 0, SEEK_SET);
	free(buf);
	
	if (off <= 0)
		return (error_handler(E_MCAVE));
	return (off);
}
