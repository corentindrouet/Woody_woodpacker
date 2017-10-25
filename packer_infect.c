#include "woody_woodpacker.h"

int		packer_infect(t_infos *infos)
{
	ssize_t		ret;
	
	lseek(infos->dst_fd, infos->cave_offset, SEEK_SET);

	ret = write(infos->dst_fd, infos->packer_code, strlen(infos->packer_code));
	if ((size_t)ret != strlen(infos->packer_code))
	{
		lseek(infos->dst_fd, 0, SEEK_SET);
		return (error_handler(E_INFECT));
	}

	printf(">> Debug: target file infected with packer source code\n");

	lseek(infos->dst_fd, 0, SEEK_SET);
	return (0);
}
