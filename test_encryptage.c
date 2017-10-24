#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

void swap(int *a, int *b)
{
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
}

int main(int argc, char **argv)
{
	char key[256];
	int tab[256];
	int i;
	int j;
	int k;
	int str_len;
	int t;
	int keyStream[512];
	char encryptedStr[256];
	char decryptedStr[256];
	int fd = open("/dev/random", O_RDONLY);

	if (fd == -1)
		return (0);
	bzero(key, 256);
	read(fd, key, 256);
	close (fd);

	if (argc != 2)
		return (0);
	i = 0;
	while (i < 256) {
		tab[i] = i;
		i++;
	}
	j = 0;
	i = 0;
	while (i < 256)
	{
		j = (j + tab[i] + key[i % 256]) % 256;
		swap(&(tab[i]), &(tab[j]));
		i++;
	}
	i = 0;
	j = 0;
	str_len = strlen(argv[1]);
	k = 0;
	while (k < str_len)
	{
		i = (i + 1) % 256;
		j = (j + tab[i]) % 256;
		swap(&(tab[i]), &(tab[j]));
		t = (tab[i] + tab[j]) % 256;
		keyStream[k] = tab[t];
		encryptedStr[k] = argv[1][k] ^ tab[k];
		k++;
	}
	i = 0;
	while (i < str_len)
	{
		encryptedStr[i] = argv[1][i] ^ keyStream[i];
		i++;
	}
	printf("%s\n", encryptedStr);
	i = 0;
	while (i < str_len)
	{
		decryptedStr[i] = encryptedStr[i] ^ keyStream[i];
		i++;
	}
	printf("%s\n", decryptedStr);
	return (0);
}
