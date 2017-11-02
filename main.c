#include "woody_woodpacker.h"

int		main(int argc, char **argv)
{
	t_datas		datas;

	datas.f_map = NULL;
	datas.f_size = 0x0;
	datas.p_map = NULL;
	datas.p_size = 0x0;
	datas.o_entry = 0x0;
	datas.n_entry = 0x0;
	datas.v_addr = 0x0;
	datas.fs_offset = 0x0;
	datas.fs_size = 0x0;
	datas.ps_offset = 0x0;
	datas.ps_size = 0x0;
	datas.c_offset = 0x0;
	datas.c_size = 0x0;
	datas.key = NULL;

	if (argc != 2)
	{
		printf("Usage: %s <binary>\n", argv[0]);
		return (-1);
	}

	if (file_copy(argv[1], TARGET_FILE) == -1)
	{
		printf("Error: unable to copy binary file\n");
		return (-1);
	}
	else
	{
		printf("Debug: %s copied to %s\n", argv[1], TARGET_FILE);
	}

	if (file_map(TARGET_FILE, &(datas.f_map), &(datas.f_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", TARGET_FILE);
		return (-1);
	}
	else
	{
		printf("Debug: target '%s' mmaped\n", TARGET_FILE);
	}

	if (file_map(PACKER_FILE, &(datas.p_map), &(datas.p_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", PACKER_FILE);
		return (-1);
	}
	else
	{
		printf("Debug: packer '%s' mmaped\n", PACKER_FILE);
	}

	if (elf64_is_valid((Elf64_Ehdr *)datas.f_map) == -1)
	{
		printf("Error: target '%s' ELF header is not valid\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}
	else
	{
		printf("Debug: target '%s' has a valid ELF header\n", TARGET_FILE);
	}

	if (elf64_is_valid((Elf64_Ehdr *)datas.p_map) == -1)
	{
		printf("Error: %s ELF header is not valid\n", PACKER_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}
	else
	{
		printf("Debug: packer '%s' has a valid ELF header\n", PACKER_FILE);
	}

	datas.o_entry = ((Elf64_Ehdr *)(datas.f_map))->e_entry;

	if (elf64_find_vaddr(datas.f_map, &(datas.v_addr)) == 0)
	{
		printf("Virtual memory address : 0x%lx\n", datas.v_addr);
	}

	if (elf64_find_sect(datas.f_map, &(datas.fs_offset), &(datas.fs_size), ".text") != -1)
	{
		printf("Section .text of %s : file offset is %lu (%lu bytes)\n", TARGET_FILE, datas.fs_offset, datas.fs_size);
		*((int *)(datas.f_map + 0x09)) = datas.fs_offset;
		*((short *)(datas.f_map + 0x0d)) = datas.fs_size;
	}

	if (elf64_find_sect(datas.p_map, &(datas.ps_offset), &(datas.ps_size), ".text") != -1)
	{
		printf("Section .text of %s : file offset is %lu (%lu bytes)\n", PACKER_FILE, datas.ps_offset, datas.ps_size);
	}

	if (elf64_find_cave(datas.f_map, datas.f_size, datas.ps_size, &(datas.c_offset), &(datas.c_size)) != -1)
	{
		printf("Code cave in %s : file offset is %lu (%lu bytes)\n", TARGET_FILE, datas.c_offset, datas.c_size);
	}

	// We move at cave offset + 256 because the first 256 bytes are used to hold the encryption key
	datas.n_entry = datas.v_addr + datas.c_offset + 256;

	printf("Old entry point address : 0x%lx\n", datas.o_entry);
	printf("New entry point address : 0x%lx\n", datas.n_entry);

	// Encrypt .text zone
	datas.key = encrypt_zone((char *)(datas.f_map + datas.fs_offset), datas.fs_size);

	// Inject encryption key
	memmove(datas.f_map + datas.c_offset, datas.key, 256);

	// Inject depacker
	memmove(datas.f_map + datas.c_offset + 256, datas.p_map + datas.ps_offset, datas.ps_size);

	// Update return address
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x2222222222222222, (uint64_t)datas.o_entry);

	// Change entry point
	((Elf64_Ehdr *)(datas.f_map))->e_entry = datas.n_entry;

	// Unmap and save changes
	file_unmap(datas.f_map, datas.f_size);
	file_unmap(datas.p_map, datas.p_size);

	return (0);
}
