#include "woody_woodpacker.h"

int			copy_file(const char *src, const char *dst)
{
	int				fd_in;
	int				fd_out;
	size_t			size;
	int				*buf;
	
	fd_in = open(src, O_RDONLY);
	fd_out = open(dst, O_RDWR | O_CREAT | O_TRUNC);
	
	if (fd_in < 0 || fd_out < 0)
	{
		printf("Error: unable to copy file, invalid file descriptor(s)\n");
		return (-1);
	}

	size = get_file_size(fd_in);
	printf("Debug: total input file size is %lu bytes\n", size);

	if ((buf = (int *)malloc(sizeof(int) * size)) == NULL)
	{
		printf("Error: unable to copy file, malloc %lu bytes failed\n", size);
		return (-1);
	}

	if (read(fd_in, buf, size) == -1)
	{
		printf("Error: unable to read from %s\n", src);
		close(fd_in);
		close(fd_out);
		free(buf);
		return (-1);
	}

	if (write(fd_out, buf, size) == -1)
	{
		printf("Error: unable to write into %s\n", dst);
		close(fd_in);
		close(fd_out);
		free(buf);
		return (-1);
	}
	
	printf("Success: file %s copied byte-for-byte into %s\n", src, dst);

	close(fd_in);
	free(buf);

	return (fd_out);
}
