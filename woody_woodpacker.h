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

int				check_elf(const char *src);
int				crypt(int fd);
size_t			get_file_size(int fd);
int				copy_file(const char *src, const char *dst);
size_t			get_file_size(int fd);
int				packer_infect(int fd, char *packer);
unsigned int	cave_miner(int fd, size_t size);

#endif
