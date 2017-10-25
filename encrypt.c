#include "woody_woodpacker.h"

static void			dumpenckey(unsigned char *key)
{
	printf(">> Debug: encryption key:\n");
	while (key != NULL && *key != '\0')
	{
		printf("%d.", *key);
		key++;
	}
	printf("\n");
}

static size_t		enckeylen(unsigned char *key)
{
	size_t			len;

	len = 0;
	while (key != NULL && *key != '\0')
	{
		key++;
		len++;
	}
	return (len);
}

static unsigned char    *convenckey(char *key)
{
	unsigned char   *key_stream;
	unsigned char	buf[4];
	int				dot;
	int				x;
	int				y;
	int				z;

	dot = 0;
	x = 0;
	while (key[x] != '\0')
		if (key[x++] == '.')
			dot++;

	key_stream = (unsigned char *)malloc(sizeof(unsigned char) * (dot + 2));
	if (key_stream == NULL)
		return (NULL);
	key_stream[dot + 1] = '\0';

	x = 0;
	y = 0;
	z = 0;
	while (key[x] != '\0')
	{
		y = 0;
		memset(buf, 0, 4);
		
		while (key[x] != '.' && key[x] != '\0')
			buf[y++] = key[x++];
		
		key_stream[z++] = atoi((char *)buf);
		
		if (key[x] == '.')
			x++;
		if (key[x] == '\0')
			break ;
	}

	return (key_stream);
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
		infos->key_stream = encrypt_zone(buf, size);
		dumpenckey(infos->key_stream);
	}

	/* Decrypt file */
	else
	{
		decrypt_zone(buf, size, infos->key_stream, enckeylen(infos->key_stream));
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

int		encrypt(t_infos *infos, char *key)
{
	char			*buf;

	if (key != NULL)
		infos->key_stream = convenckey(key);

	buf = mmap(0, infos->file_size, PROT_READ, MAP_PRIVATE, infos->dst_fd, 0);
	if (buf == MAP_FAILED)
	{
		printf("Error: mmap failed on encryption\n");
		return (-1);
	}
	return (encrypt_section(infos, buf));
}
