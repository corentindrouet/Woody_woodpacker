#include "woody_woodpacker.h"

unsigned int	cave_miner(int fd, size_t size)
{
	char		*buf;
	size_t		len;
	size_t		start;
	size_t		end;
	size_t		off;
	size_t		max;

	len = get_file_size(fd);
	start = 0;
	end = 0;
	off = 0;
	max = 0;

	// Debug
	printf(">> Searching for code cave:\nRequired size: %lu\nTotal file length: %lu\n", size, len);

	// Malloc file
	if ((buf = (char *)malloc(sizeof(char) * len)) == NULL)
		return (-1);
	
	// Fill-in buffer with file content
	if (read(fd, buf, len) == -1)
		return (-1);

	// Until we did not read EOF
	while ((start != len))
	{
		// If suitable length is found
		if (end != 0 && max >= size)
			break ;

		// Compute file offsets
		end = start;
		if (buf[start] != 0)
			start++;
		else
		{
			end = start;
			while (end != len && buf[end] == 0)
				end++;
			if ((end - start) > max)
			{
				off = start;
				max = end - start;
			}
			start = end + 1;
		}
	}
	
	// If suitable length found
	if (off && max)
		printf("\n[*] Found code cave\nStart is at %lu\nAvailable size is %lu\n\n", off, max);

	free(buf);
	return (off);
}
