#include "woody_woodpacker.h"

static int	update_entry(int fd, unsigned int offset)
{
	char		*buf;
	size_t		size;
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*phdr;
	int			cmpt;

	(void)offset;
	size = get_file_size(fd);
	buf = (char *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED)
		return (-1);
	ehdr = (Elf64_Ehdr *)buf;

	printf("e_entry=%lx\n", ehdr->e_entry);
	printf("e_phnum=%d\n", ehdr->e_phnum);

	cmpt = 0;
	while (cmpt < ehdr->e_phnum)
	{
		phdr = (Elf64_Phdr *)(buf + ehdr->e_phoff + (cmpt * sizeof(Elf64_Phdr)));
		if (phdr->p_type == 1)
		{
			printf("PT_LOAD virtual address: %lu\n", phdr->p_vaddr);
			printf("PT_LOAD new virtual address : %lu\n", phdr->p_vaddr + offset);
		}
		cmpt++;
	}

	munmap(buf, size);
	return (0);
}

int		main(int argc, char **argv)
{
	/* Holds the copied binary fd */
	int				fd;
	/* Holds the asm packer code */
	char			packer[] = "Lorem ipsum dolor sit amet";
	/* Packer file offset */
	unsigned int	offset;

	/* Test arguments */
	if (argc != 3)
	{
		printf("Error: ./woody_woodpacker <src> <dst>\n");
		return (1);
	}

	/* Check ELF magic number (works for arch 64) */
	if (check_elf(argv[1]) == -1)
	{
		printf("Error: %s is not an elf 64 valid format\n", argv[1]);
		return (1);
	}

	/* Make a copy of the binary input file to a new file byte-for-byte identical */
	if ((fd = copy_file(argv[1], argv[2])) == -1)
	{
		printf("Error: %s copy failed\n", argv[1]);
		return (1);
	}

	/* Find a code cave that can fit our asm packer code and write it there */
	if ((offset = packer_infect(fd, packer)) == 0)
	{
		printf("Error: %s does not contain code cave large enough\n", argv[1]);
		close(fd);
		return (1);
	}
	else
	{
		printf("Debug: packer code wrote at file offset %u\n", offset);
	}

	/* Store the current e_entry and compute the new e_entry (code cave offset + base image address) */
	update_entry(fd, offset);

	/* Encrypt .text section */
	/*if (crypt(fd) == -1)
	{
		printf("Error: file encryption failed\n");
		close(fd);
		return (1);
	}*/

	close(fd);
	return (0);
}
