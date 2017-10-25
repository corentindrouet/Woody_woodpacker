#include "woody_woodpacker.h"

int		file_size(t_infos *infos)
{
	off_t	ret;
	
	lseek(infos->src_fd, 0, SEEK_SET);
	ret = lseek(infos->src_fd, 0, SEEK_END);
	lseek(infos->src_fd, 0, SEEK_SET);
	infos->file_size = (ret <= 0 ) ? 0 : (uint32_t)ret;
	return (ret);
}
