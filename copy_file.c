#include "woody_woodpacker.h"

int			copy_file(t_infos *infos)
{
	int			ret;
	int			*buf;

	if ((buf = (int *)malloc(sizeof(int) * infos->file_size)) == NULL)
	{
		return (error_handler(E_COPY));
	}

	if ((ret = read(infos->src_fd, buf, infos->file_size)) != infos->file_size)
	{
		free(buf);
		return (error_handler(E_COPY));
	}

	lseek(infos->src_fd, 0, SEEK_SET);

	if ((ret = write(infos->dst_fd, buf, infos->file_size)) != infos->file_size)
	{
		free(buf);
		return (error_handler(E_COPY));
	}
	
	lseek(infos->src_fd, 0, SEEK_SET);

	printf(">> Debug: source file copied\n");

	free(buf);
	return (0);
}
