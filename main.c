#include "woody_woodpacker.h"

int				main(int argc, char **argv)
{
	int			fd;
	char		packer[] = "Lorem ipsum dolor sit amet, \
	consectetur adipiscing elit. Morbi ut fermentum elit, \
	vel blandit libero. Pellentesque ac magna posuere, \
	ullamcorper elit ac, viverra mi. Ut et pellentesque \
	libero. Curabitur ac gravida diam, in aliquet tellus. \
	Integer eget arcu ipsum. Duis vitae arcu in elit porta \
	ultricies nec scelerisque sem. Nulla ac quam nisl";

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

	if (packer_infect(fd, packer) == -1)
	{
		printf("Error: %s does not contain code cave large enough\n", argv[1]);
		close(fd);
		return (1);
	}

	/*if (crypt(fd) == -1)
	{
		printf("Error: file encryption failed\n");
		close(fd);
		return (1);
	}*/

	close(fd);
	return (0);
}
