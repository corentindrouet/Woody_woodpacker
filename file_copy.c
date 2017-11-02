#include "woody_woodpacker.h"

static int	file_copy_free(int fd_src, int fd_dst, char **buf)
{
	if (*buf != NULL)
	{
		free(*buf);
		*buf = NULL;
	}
	close(fd_src);
	close(fd_dst);
	return (-1);
}

int		file_copy(const char *src, const char *dst)
{
	int		fd_src;
	int		fd_dst;
	size_t	size;
	ssize_t	ret;
	char	*buf;

	buf = NULL;
	fd_src = open(src, O_RDONLY);
	fd_dst = open(dst, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd_src < 0 || fd_dst < 0)
		return (file_copy_free(fd_src, fd_dst, &buf));
	size = file_size(fd_src);
	if (!(buf = (char *)malloc(sizeof(char) * size)))
		return (file_copy_free(fd_src, fd_dst, &buf));
	ret = read(fd_src, buf, size);
	if (ret < 0 || (size_t)ret != size)
		return (file_copy_free(fd_src, fd_dst, &buf));
	ret = write(fd_dst, buf, size);
	if (ret < 0 || (size_t)ret != size)
		return (file_copy_free(fd_src, fd_dst, &buf));
	file_copy_free(fd_src, fd_dst, &buf);
	return (0);
}
