#include "woody_woodpacker.h"

static unsigned int		code_cave(int fd, size_t size)
{
	char				*buf;
	size_t				len;
	size_t				start;
	size_t				end;
	size_t				off;
	size_t				max;


	len = get_file_size(fd);
	start = 0;
	end = 0;
	off = 0;
	max = 0;

	printf("Finding code cave:\nFit size: %lu\nFile length: %lu\n", size, len);

	if ((buf = (char *)malloc(sizeof(char) * len)) == NULL)
	{
		printf("error: code_cave: 1\n");
		return (-1);
	}
	
	if (read(fd, buf, len) == -1)
	{
		printf("error: code_cave: 2\n");
		return (-1);
	}
	
	while ((start != len))
	{
		end = start;

		if (end != 0 && max >= size)
		{
			printf("debug: code_cave: brake !\n");
			break ;
		}

		if (buf[start] != 0)
		{
			write(1, "-", 1);
			start++;
		}
		else
		{
			end = start;
			
			while (end != len && buf[end] == 0)
			{
				write(1, ".", 1);
				end++;
			}
			
			printf("Info: start is at %lu, end is at %lu, diff is %lu\n", start, end, end - start);

			if ((end - start) > max)
			{
				off = start;
				max = end - start;
			}

			start = end + 1;
		
			//printf("Gap found : offset=%lu, length=%lu\n", start, end - start);
		}
	}
	
	free(buf);
	return (off);
}

int					packer_infect(int fd, char *packer)
{
	unsigned int	offset;
	//unsigned int	start;
	//unsigned int	pos;

	lseek(fd, 0, SEEK_SET);
	offset = code_cave(fd, strlen(packer));
	if (offset == 0)
		return (-1);

	/*start = offset;
	pos = 0;

	lseek(fd, start, SEEK_SET);

	if (write(fd, packer, strlen(packer)) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: write failed\n");
		free(buffer);
		return (-1);
	}

	lseek(fd, 0, SEEK_SET);*/
	return (0);
}
