#include "woody_woodpacker.h"

static int			keyfile_read(t_infos *infos, const char *keyfile)
{
	int				fd;
	ssize_t			ret;
	unsigned char	*buf;

	printf(">> Debug: reading from 'keyfile'\n");

	if (!keyfile)
	{
		printf("Error: no 'keyfile' target specified\n");
		return (-1);
	}
	if ((fd = open(keyfile, O_RDONLY)) == -1)
	{
		printf("Error: unable to open 'keyfile'\n");
		return (-1);
	}
	if (!(buf = (unsigned char *)malloc(sizeof(unsigned char) * 256)))
	{
		printf("Error: unable to malloc buffer for 'keyfile'\n");
		close(fd);
		return (-1);
	}
	memset(buf, 0, 256);
	if ((ret = read(fd, buf, 256)) <= 0)
	{
		printf("Error: unable to read from 'keyfile'\n");
		close(fd);
		free(buf);
		return (-1);
	}
	printf(">> Debug: %ld bytes read from 'keyfile'\n", ret);
	close(fd);
	infos->key_stream = buf;
	infos->key_length = (size_t)ret;
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

static int			decrypt_process(t_infos *infos, size_t off, size_t size)
{
	int				fd;
	char			*buf;

	fd = infos->dst_fd;

	printf(">> Debug: processing decryption ...\n");

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

	keyfile_dump(infos->key_stream);
	decrypt_zone(buf, size, infos->key_stream, infos->key_length);

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

static int			decrypt_section(t_infos *infos, char *buf)
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
			return (decrypt_process(infos, st.sh_offset, st.sh_size));
		i++;
	}
	printf("Error: no section .text found on encryption\n");
	return (-1);
}

int		decrypt(t_infos *infos, const char *keyfile)
{
	char			*buf;

	if (keyfile_read(infos, keyfile) == -1)
	{
		printf("Error: retreiving 'keyfile' content failed\n");
		return (-1);
	}

	buf = mmap(0, infos->file_size, PROT_READ, MAP_PRIVATE, infos->dst_fd, 0);
	if (buf == MAP_FAILED)
	{
		printf("Error: mmap failed on encryption\n");
		return (-1);
	}
	return (decrypt_section(infos, buf));
}
