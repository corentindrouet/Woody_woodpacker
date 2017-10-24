#include "woody_woodpacker.h"

static void			print_hex(int *buffer, unsigned int size)
{
	unsigned int	i;

	printf("Input :\n");
	i = 0;
	while (i < size)
	{
		printf("%x", buffer[i]);
		i++;
	}
	printf("\n");
}

static int			crypt_xor(int fd, char *key, unsigned int offset, unsigned int size)
{
	int				*buffer;
	unsigned int	start;
	unsigned int	bufpos;
	unsigned int	keypos;

	start = offset;
	bufpos = 0;
	keypos = 0;

	// Place cursor at the begening of the section
	lseek(fd, start, SEEK_SET);

	// Malloc a new buffer
	if ((buffer = (int *)malloc(sizeof(int) * size)) == NULL)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: failed to malloc buffer\n");
		return (-1);
	}

	// Fill in the buffer
	if (read(fd, buffer, size) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: read failed\n");
		free(buffer);
		return (-1);
	}

	// Re-place cursor at the begening of the section (read moved it 'size' bytes)
	lseek(fd, start, SEEK_SET);
	
	// debug
	print_hex(buffer, size);

	// Alter (XOR) buffer bytes
	while (offset < (start + size))
	{
		buffer[bufpos] ^= key[keypos];
		offset++;
		bufpos++;
		keypos++;
		if (keypos == strlen(key))
			keypos = 0;
	}
	
	// debug
	print_hex(buffer, size);

	// Replace the section with xor-ed buffer
	if (write(fd, buffer, size) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: write failed\n");
		free(buffer);
		return (-1);
	}

	// Re-place cursor at the begening of file
	lseek(fd, 0, SEEK_SET);

	// Free malloced buffer
	free(buffer);

	// Return OK
	return (0);
}

static int			crypt_section(int fd, char *buf)
{
	Elf64_Ehdr		*e;
	Elf64_Shdr		*s;
	Elf64_Shdr		st;
	char			*stab;
	int				i;

	e = (Elf64_Ehdr *)buf;

	s = (Elf64_Shdr *)(buf + e->e_shoff);
	st = s[e->e_shstrndx];
	stab = (char *)(buf + st.sh_offset);

	i = 0;
	while (i < e->e_shnum)
	{
		st = s[i];
		if (!strcmp(stab + st.sh_name, ".text"))
		{
			printf("Section name=%s size=%lu offset=%lu\n", stab + st.sh_name, st.sh_size, st.sh_offset);
			return (crypt_xor(fd, "rludosan", st.sh_offset, st.sh_size));
		}
		i++;
	}
	return (-1);
}

int				crypt(int fd)
{
	char		*buf;

	buf = mmap(0, get_file_size(fd), PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED)
		return (-1);
	return (crypt_section(fd, buf));
}
