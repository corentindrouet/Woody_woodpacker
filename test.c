
void	decrypt_zone(char *zone, unsigned long zone_size, unsigned char *key, unsigned long key_size)
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
		tab[i] = tab[i] + tab[j];
		tab[j] = tab[i] - tab[j];
		tab[i] = tab[i] - tab[j];
	}
	i = 0;
	j = 0;
	k = -1;
	while (++k < zone_size)
	{
		i = (i + 1) % 256;
		j = (j + tab[i]) % 256;
		tab[i] = tab[i] + tab[j];
		tab[j] = tab[i] - tab[j];
		tab[i] = tab[i] - tab[j];
		j = (tab[i] + tab[j]) % 256;
		zone[k] = zone[k] ^ tab[j];
	}
}
