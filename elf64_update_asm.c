#include "woody_woodpacker.h"

int		elf64_update_asm(void *m, size_t len, uint64_t pat, uint64_t val)
{
	unsigned char	*p;
	uint64_t		n;
	size_t			i;

	p = (unsigned char *)m;
	i = 0;
	while (i < len)
	{
		n = *((uint64_t *)(p + i));
		if (n == pat)
		{
			printf("++ replace address found !\n");
			*((uint64_t *)(p + i)) = val;
			return (0);
		}
		i++;
	}
	return (-1);
}
