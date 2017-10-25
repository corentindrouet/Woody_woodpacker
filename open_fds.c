#include "woody_woodpacker.h"

int			open_fds(t_infos *infos, const char *src, const char *dst)
{
	if (infos == NULL || src == NULL || dst == NULL)
		return (-1);
	
	infos->src_fd = open(src, O_RDONLY);
	infos->dst_fd = open(dst, O_RDWR | O_CREAT | O_TRUNC);

	if (infos->src_fd < 0 || infos->dst_fd < 0)
	{
		close(infos->src_fd);
		close(infos->dst_fd);
		return (-1);
	}

	return (0);
}
