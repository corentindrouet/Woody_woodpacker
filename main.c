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

	if (elf64_is_exec((Elf64_Ehdr *)datas.f_map) == 0 || elf64_is_dyn((Elf64_Ehdr *)datas.f_map) == 0)
		printf("Debug: target '%s' has a valid ELF header\n", TARGET_FILE);
	else
	{
		printf("Error: target '%s' ELF header is not valid\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	if (elf64_is_rel((Elf64_Ehdr *)datas.p_map) == -1)
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
		printf("Virtual memory address : %#lx\n", datas.v_addr);
	}

	if (elf64_find_sect(datas.f_map, &(datas.fs_offset), &(datas.fs_size), ".text") != -1)
	{
		printf("Section .text of %s : file offset is %lu (%lu - %#lx bytes)\n", TARGET_FILE, datas.fs_offset, datas.fs_size, datas.fs_size);
		*((int *)(datas.f_map + 0x09)) = datas.fs_offset;
		*((short *)(datas.f_map + 0x0d)) = datas.fs_size;
	}

	if (elf64_find_sect(datas.p_map, &(datas.ps_offset), &(datas.ps_size), ".text") != -1)
	{
		printf("Section .text of %s : file offset is %lu (%lu - %#lx bytes)\n", PACKER_FILE, datas.ps_offset, datas.ps_size, datas.ps_size);
	}

	if (elf64_find_cave(datas.f_map, datas.f_size, datas.ps_size, &(datas.c_offset), &(datas.c_size)) != -1)
	{
		printf("Code cave in %s : file offset is %lu (%lu bytes)\n", TARGET_FILE, datas.c_offset, datas.c_size);
	}

	// We move at cave offset + 256 because the first 256 bytes are used to hold the encryption key
	datas.n_entry = datas.v_addr + datas.c_offset + 256;

	printf("Old entry point address : %#lx\n", datas.o_entry);
	printf("New entry point address : %#lx\n", datas.n_entry);

	// Encrypt .text zone
	datas.key = encrypt_zone((char *)(datas.f_map + datas.fs_offset), datas.fs_size);

	// Inject encryption key
	memmove(datas.f_map + datas.c_offset, datas.key, 256);
	printf("Encryption key injected\n");

	// Inject depacker
	memmove(datas.f_map + datas.c_offset + 256, datas.p_map + datas.ps_offset, datas.ps_size);
	printf("Packer code injected\n");

	// Update return address
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x1111111111111111, (uint64_t)(datas.fs_offset + datas.v_addr));
	printf("ASM update : .text section address\n");

	// Update return address
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x2222222222222222, (uint64_t)datas.fs_size);
	printf("ASM update : .text section size\n");

	// Update return address
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x3333333333333333, (uint64_t)datas.n_entry - 256);
	printf("ASM update : key location address\n");

	// Update return address
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x4444444444444444, (uint64_t)datas.o_entry);
	printf("ASM update : return address\n");

	uint64_t tmp_p = datas.v_addr + datas.fs_offset;
	uint64_t tmp_l = datas.v_addr + datas.fs_offset + datas.fs_size;

	printf("Page align address is : 0x%lx\n", (uint64_t)(tmp_p & -4096));
	printf("Page align address size : 0x%lx\n", (uint64_t)(tmp_l - (tmp_p & -4096)));

	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x5555555555555555, (uint64_t)(tmp_p & -4096));
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x6666666666666666, (uint64_t)(tmp_l - (tmp_p & -4096)));

	// Change entry point
	((Elf64_Ehdr *)(datas.f_map))->e_entry = datas.n_entry;

	// Unmap and save changes
	file_unmap(datas.f_map, datas.f_size);
	file_unmap(datas.p_map, datas.p_size);

	return (0);
}
