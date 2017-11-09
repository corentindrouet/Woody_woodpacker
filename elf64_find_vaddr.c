#include "woody_woodpacker.h"

int		elf64_find_vaddr(void *f_map, uint64_t *v_addr, size_t ps_size, off_t *c_offset, size_t *c_size)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*c_phdr;
	Elf64_Phdr	*n_phdr;
	int			i;

	ehdr = (Elf64_Ehdr *)f_map;
	i = 0;
	while (i < ehdr->e_phnum)
	{
		c_phdr = (Elf64_Phdr *)(f_map + ehdr->e_phoff + (i * sizeof(Elf64_Phdr)));
		if (c_phdr->p_type == PT_LOAD && c_phdr->p_flags & 0x011)
		{
			printf("[+] PT_LOAD segment with execution rights found at file offset %lu (virtual 0x%lx) (%lu bytes)\n", c_phdr->p_offset, c_phdr->p_vaddr, c_phdr->p_filesz);

			// Virtual memory base address
			*v_addr = c_phdr->p_vaddr;

			// Used to calculate inbetween segment padding
			if ((i + 1) < ehdr->e_phnum)
			{
				// Next segment
				n_phdr = (Elf64_Phdr *)(f_map + ehdr->e_phoff + ((i + 1) * sizeof(Elf64_Phdr)));

				printf("[+] Next segment is at file offset %lu\n", n_phdr->p_offset);
				printf("[+] In between segments padding is %lu bytes\n", n_phdr->p_offset - (c_phdr->p_offset + c_phdr->p_filesz));
				printf("[+] Packer and encryption key require %lu bytes\n", ps_size);

				if ((n_phdr->p_offset - (c_phdr->p_offset + c_phdr->p_filesz)) > ps_size)
				{
					// Save offset and size available for injection
					*c_offset = c_phdr->p_offset + c_phdr->p_filesz;
					*c_size = n_phdr->p_offset - (c_phdr->p_offset + c_phdr->p_filesz);

					// Update segment (old size + packer size)
					// c_phdr->p_filesz += ps_size;
					// c_phdr->p_memsz += ps_size;

					printf("[+] Extending PT_LOAD(exec) segment up to %lu bytes\n", c_phdr->p_filesz);
					return (0);
				}
				else
					return (-1);
			}
		}
		i++;
	}
	return (-1);
}
