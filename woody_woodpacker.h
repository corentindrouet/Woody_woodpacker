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
#define PACKER_FILE	"packer"

/*
** This structures holds all the basics infos
*/

typedef struct		s_datas
{
	void			*f_map;			/* Binary mmap pointer */
	size_t			f_size;			/* Binary file size */
	void			*p_map;			/* Packer mmap pointer */
	size_t			p_size;			/* Packer file size */
	uint64_t		o_entry;		/* Old entry point */
	uint64_t		n_entry;		/* New entry point */
	uint64_t		v_addr;			/* Virtual base address */
	off_t			fs_offset;		/* Binary section offset */
	size_t			fs_size;		/* Binary section size */
	off_t			ps_offset;		/* Packer section offset */
	size_t			ps_size;		/* Packer section size */
	off_t			c_offset;		/* Cave file offset */
	size_t			c_size;			/* Cave size */
	unsigned char	*key;			/* Encryption key */
}					t_datas;

/*
** Tests
*/

void				ft_xor(void *p, size_t l);

/*
** Elf64
*/

int					elf64_update_asm(void *m, size_t len, uint64_t pat, uint64_t val);
int					elf64_find_cave(void *f_map, size_t f_size, size_t ps_size, off_t *c_offset, size_t *c_size);
int					elf64_find_sect(void *f_map, off_t *s_offset, size_t *s_size, const char *sect);
int					elf64_find_vaddr(void *f_map, uint64_t *v_addr);
int					elf64_is_valid(Elf64_Ehdr *ehdr);
int					elf64_is_rel(Elf64_Ehdr *ehdr);
int					elf64_is_exec(Elf64_Ehdr *ehdr);
int					elf64_is_dyn(Elf64_Ehdr *ehdr);

/*
** File
*/

size_t				file_size(int fd);
int					file_unmap(void *f_map, size_t f_size);
int					file_map(const char *name, void **f_map, size_t *f_size);
int					file_copy(const char *src, const char *dst);

/*
** Encryption
*/

void				swap(int *a, int *b);
unsigned char		*encrypt_zone(char *zone, size_t size);
void				*get_random_key(size_t size);

#endif
