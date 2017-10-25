#include "woody_woodpacker.h"

int		main(int argc, char **argv)
{
	t_infos			*infos;

	/* Test arguments */
	if (argc != 3)
		return (error_handler(E_USAGE));

	/* Init infos structure */
	infos = infos_init(argv[1], argv[2]);
	if (infos == NULL)
		return (-1);

	/* Check ELF magic number */
	if (check_elf(infos->src_fd) == -1)
	{
		infos_destroy(infos);
		return (-1);
	}

	/* Copy binary source file to destination file */
	if (copy_file(infos) == -1)
	{
		infos_destroy(infos);
		return (-1);
	}

	/* Find a code cave */
	if (mine_cave(infos) <= 0)
	{
		infos_destroy(infos);
		return (-1);
	}

	/* Infect packer */
	if (packer_infect(infos) == -1)
	{
		infos_destroy(infos);
		return (-1);
	}

	infos_destroy(infos);
	return (0);
}
