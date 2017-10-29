#ifndef WOODY_WOODPACKER_H
#define WOODY_WOODPACKER_H

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define E_USAGE		1
#define E_FDOPEN	2
#define E_ELFHDR	3
#define E_FSIZE		4
#define E_COPY		5
#define E_MCAVE		6
#define E_INFECT	7

#define TARGET_FILE	"woody"

/*
** This structures holds all the basics infos
*/

typedef struct		s_infos
{
	int				src_fd;
	int				dst_fd;
	ssize_t			file_size;
	size_t			old_entry;
	size_t			new_entry;
	size_t			image_base;
	ssize_t			cave_offset;
	size_t			textsect_offset;
	size_t			textsect_size;
	unsigned char	*key_stream;
	size_t			key_length;
	char			*packer_code;
}					t_infos;

/*
** Infos struct init and destroy
*/
void				infos_destroy(t_infos *infos);
t_infos				*infos_init(const char *src, const char *dst);

/*
** Open src and dst file descriptors
*/
int					open_fds(t_infos *infos, const char *src, const char *dst);

/*
** Error messages handler
*/
int					error_handler(int code);

/*
** Checks if file is ELF64
*/
int					check_elf(int fd);

/*
** Computes the file size in bytes
*/
int					file_size(t_infos *infos);

/*
** Copies binary source file
*/
int					copy_file(t_infos *infos);

/*
** Find a code cave
*/
int					mine_cave(t_infos *infos);

/*
** Write packer code into target file
*/
int					packer_infect(t_infos *infos);

/*
** Encryption / Decription
*/

int					encrypt(t_infos *infos);
int					decrypt(t_infos *infos, const char *keyfile);


unsigned char		*encrypt_zone(char *zone, size_t size);
void				decrypt_zone(char *zone, size_t zone_size, unsigned char *key, size_t key_size);
void				*get_random_key(size_t size);
void				swap(int *a, int *b);

#endif
