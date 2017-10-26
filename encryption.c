#include "woody_woodpacker.h"

void	swap(int *a, int *b)
{
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
}

/**
 * This function take a pointer who is the address of the zone to encrypt, and a size, needed to know
 * how many bytes to encrypts. It return the random key use to encrypt the zone.
 */

unsigned char	*encrypt_zone(char *zone, size_t size)
{
	unsigned char	*key;
	int				tab[256];
	int				i;
	int				j;
	size_t			k;

	if (!zone || !size || !(key = get_random_key(256)))
		return (0);
	i = -1;
	printf("> First encryption step, key:%s\n", key);
	while (++i < 256)
		tab[i] = i;
	i = -1;
	j = 0;
	printf("> Second encryption step\n");
	while (++i < 256)
	{
		j = (j + tab[i] + key[i % 256]) % 256;
		swap(&(tab[i]), &(tab[j]));
	}
	i = 0;
	j = 0;
	k = 0;
	printf("> Third encryption step\n");
	while (k < size)
	{
		i = (i + 1) % 256;
		j = (j + tab[i]) % 256;
		swap(&(tab[i]), &(tab[j]));
		j = (tab[i] + tab[j]) % 256;
		zone[k] = zone[k] ^ tab[j];
		k++;
	}
	printf("> Done\n");
	return (key);
}

void	*get_random_key(size_t size)
{
	void	*buffer;
	int		fd;
	int		numberRandomBytesReaded;

	numberRandomBytesReaded = 0;
	if ((fd = open("/dev/urandom", O_RDONLY)) == -1)
		return (NULL);
	if (!(buffer = malloc(size + 1)))
		return (NULL);
	bzero(buffer, size + 1);
	while (numberRandomBytesReaded < 256)
	{	
		read(fd, (buffer + numberRandomBytesReaded), size - numberRandomBytesReaded);
		numberRandomBytesReaded = ft_strlen(buffer);
	}
	close(fd);
	return (buffer);
}
