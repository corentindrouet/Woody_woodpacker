#include "woody_woodpacker.h"

static size_t		keyfile_length(unsigned char *key)
{
	size_t			len;

	len = 0;
	while (key && *key != 0)
	{
		len++;
		key++;
	}
	return (len);
}

static int			keyfile_write(unsigned char *key)
{
	ssize_t			ret;
	int				fd;

	printf(">> Debug: saving key into 'keyfile'\n");

	if ((fd = open("keyfile", O_RDWR | O_CREAT | O_TRUNC)) == -1)
	{
		printf("Error: unable to open 'keyfile' for writing\n");
		return (-1);
	}
	ret = write(fd, key, keyfile_length(key));
	printf(">> Debug: %ld / %lu bytes written into 'keyfile'\n", ret, keyfile_length(key));
	if ((size_t)ret != keyfile_length(key))
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static void			keyfile_dump(unsigned char *key)
{
	printf(">> Debug: encryption key:\n");
	while (key != NULL && *key != '\0')
	{
		printf("%d.", *key);
		key++;
	}
	printf("\n");
}

static int			encrypt_process(t_infos *infos, size_t off, size_t size)
{
	int				fd;
	char			*buf;

	fd = infos->dst_fd;

	printf(">> Debug: processing encryption ...\n");

	/* Place cursor at the begening of the section */
	lseek(fd, off, SEEK_SET);

	/* Malloc new buffer */
	buf = (char *)malloc(sizeof(char) * size);
	if (buf == NULL)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: buffer allocation on encryption failed\n");
		return (-1);
	}

	/* Read section */
	if (read(fd, buf, size) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: read section on encryption failed\n");
		free(buf);
		return (-1);
	}

	/* Encrypt file */
	if (infos->key_stream == NULL)
	{
		// Save key
		infos->key_stream = encrypt_zone(buf, size);
		// Print on screen
		keyfile_dump(infos->key_stream);
		// Save key into file
		keyfile_write(infos->key_stream);
	}

	/* Re-place cursor at the begening of the section (read moved it 'size' bytes) */
	lseek(fd, off, SEEK_SET);

	/* Replace the section with RC4-ed buffer */
	if (write(fd, buf, size) == -1)
	{
		lseek(fd, 0, SEEK_SET);
		printf("Error: write section on encryption failed\n");
		free(buf);
		return (-1);
	}

	/* Re-place cursor at the begening of file */
	lseek(fd, 0, SEEK_SET);

	free(buf);
	return (0);
}

static int			encrypt_section(t_infos *infos, char *buf)
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
			return (encrypt_process(infos, st.sh_offset, st.sh_size));
		i++;
	}
	printf("Error: no section .text found on encryption\n");
	return (-1);
}

int		encrypt(t_infos *infos)
{
	char			*buf;

	buf = mmap(0, infos->file_size, PROT_READ, MAP_PRIVATE, infos->dst_fd, 0);
	if (buf == MAP_FAILED)
	{
		printf("Error: mmap failed on encryption\n");
		return (-1);
	}
	return (encrypt_section(infos, buf));
}
