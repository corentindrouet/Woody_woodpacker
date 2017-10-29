#include "woody_woodpacker.h"

void		infos_destroy(t_infos *infos)
{
	if (infos != NULL)
	{
		close(infos->src_fd);
		close(infos->dst_fd);
		if (infos->key_stream != NULL)
			free(infos->key_stream);
		if (infos->packer_code != NULL)
			free(infos->packer_code);
		free(infos);
		infos = NULL;
	}
}

t_infos		*infos_init(const char *src, const char *dst)
{
	t_infos		*infos;

	infos = (t_infos *)malloc(sizeof(t_infos));
	if (infos == NULL)
		return (NULL);

	infos->src_fd = -1;
	infos->dst_fd = -1;
	infos->file_size = 0;
	infos->old_entry = 0x0;
	infos->new_entry = 0x0;
	infos->image_base = 0x0;
	infos->cave_offset = 0x0;
	infos->textsect_offset = 0x0;
	infos->textsect_size = 0x0;
	infos->key_stream = NULL;
	infos->key_length = 0x0;
	infos->packer_code = NULL;

	if (open_fds(infos, src, dst) == -1)
	{
		error_handler(E_FDOPEN);
		free(infos);
		return (NULL);
	}
	else
		printf(">> Debug: source fd is %d and destination fd is %d\n", infos->src_fd, infos->dst_fd);

	if (file_size(infos) <= 0)
	{
		error_handler(E_FSIZE);
		free(infos);
		return (NULL);
	}
	else
		printf(">> Debug: source file size is %ld bytes\n", infos->file_size);

	/* FOR DEV PURPOSE ONLY */
	infos->packer_code = (char *)malloc(sizeof(char) * 685);
	if (infos->packer_code == NULL)
	{
		printf("Error: cannot malloc packer code string\n");
		free(infos);
		return (NULL);
	}
	else
	{
		memcpy(infos->packer_code, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse rhoncus congue dui, lobortis tristique felis bibendum et. Ut ut nunc efficitur, pellentesque libero ut, volutpat enim. Ut finibus pretium sem sit amet pulvinar. Nulla sodales ornare efficitur. Maecenas faucibus euismod libero a placerat. Aliquam eget tincidunt nisl, varius imperdiet nunc. Nulla vehicula leo non ex consectetur, eget gravida tortor consequat. In pellentesque elit et fringilla tempor. Phasellus at lorem fermentum, gravida sem at, consectetur eros. Integer vel ex lectus. Nullam egestas, neque lobortis euismod dignissim, massa mauris commodo nunc, ac rhoncus neque magna id ex. Suspendisse potenti.", 685);
	}

	return (infos);
}
