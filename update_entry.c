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
