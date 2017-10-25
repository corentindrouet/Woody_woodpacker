#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

void swap(int *a, int *b)
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
	int		tab[256];
	int		i;
	int		j;
	int		k;

	if (!zone || !size || !(key = get_random_key(256)))
		return (0);
	i = -1;
	while (++i < 256)
		tab[i] = i;
	i = -1;
	j = 0;
	while (++i < 256)
	{
		j = (j + tab[i] + key[i % 256]) % 256;
		swap(&(tab[i]), &(tab[j]));
	}
	i = 0;
	j = 0;
	k = -1;
	while (++k < size)
	{
		i = (i + 1) % 256;
		j = (j + tab[i]) % 256;
		swap(&(tab[i]), &(tab[j]));
		j = (tab[i] + tab[j]) % 256;
		zone[k] = zone[k] ^ tab[j];
	}
	return (key);
}

void	*get_random_key(size_t size)
{
	void	*buffer;
	int		fd;

	if ((fd = open("/dev/random", O_RDONLY)) == -1)
		return (NULL);
	if (!(buffer = malloc(size + 1)))
		return (NULL);
	bzero(buffer, size + 1);
	read(fd, buffer, size);
	close(fd);
	return (buffer);
}

void	decrypt_zone(char *zone, size_t zone_size, unsigned char *key, size_t key_size)
{
	int		tab[256];
	int		i;
	int		j;
	int		k;

	i = -1;
	while (++i < 4)
		if (((unsigned long)*(&(zone) - i)) == 0)
			return ;
	i = -1;
	while (++i < 256)
		tab[i] = i;
	i = -1;
	j = 0;
	while (++i < 256)
	{
		j = (j + tab[i] + key[i % key_size]) % 256;
		swap(&(tab[i]), &(tab[j]));
	}
	i = 0;
	j = 0;
	k = -1;
	while (++k < zone_size)
	{
		i = (i + 1) % 256;
		j = (j + tab[i]) % 256;
		swap(&(tab[i]), &(tab[j]));
		j = (tab[i] + tab[j]) % 256;
		zone[k] = zone[k] ^ tab[j];
	}
}
