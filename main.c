#include "woody_woodpacker.h"

int				main(int argc, char **argv)
{
	int			fd;

	if (argc != 3)
	{
		printf("Error: ./woody_woodpacker <src> <dst>\n");
		return (1);
	}

	if (check_elf(argv[1]) == -1)
	{
		printf("Error: %s is not an elf 64 valid format\n", argv[1]);
		return (1);
	}

	if ((fd = copy_file(argv[1], argv[2])) == -1)
	{
		printf("Error: %s copy failed\n", argv[1]);
		return (1);
	}
	else
	{
		printf("File copied !\n");
	}

	if (packer_infect(fd, "this is some code to be tested") == -1)
	{
		printf("Error: %s does not contain code cave large enough\n", argv[1]);
		close(fd);
		return (1);
	}
	else
	{
		printf("Code cave found !\n");
	}

	/*if (crypt(fd) == -1)
	{
		printf("Error: (de)crypt failed\n");
		close(fd);
		return (1);
	}
	else
	{
		printf("File (de)crypted !\n");
	}*/

	close(fd);
	return (0);
}
