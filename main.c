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

	if (file_map(TARGET_FILE, &(datas.f_map), &(datas.f_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", TARGET_FILE);
		return (-1);
	}

	if (file_map(PACKER_FILE, &(datas.p_map), &(datas.p_size)) == -1)
	{
		printf("Error: unable to mmap : %s\n", PACKER_FILE);
		return (-1);
	}

	if (elf64_is_exec((Elf64_Ehdr *)datas.f_map) == -1)
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

	datas.o_entry = ((Elf64_Ehdr *)(datas.f_map))->e_entry;

	if (elf64_find_sect(datas.f_map, &(datas.fs_offset), &(datas.fs_size), ".text") == -1)
	{
		printf("Error: %s .text section not found\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	if (elf64_find_sect(datas.p_map, &(datas.ps_offset), &(datas.ps_size), ".text") == -1)
	{
		printf("Error: %s .text section not found\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	if (elf64_find_vaddr(datas.f_map, &(datas.v_addr), datas.ps_size + 256, &(datas.c_offset), &(datas.c_size)) == -1)
	{
		printf("Error: %s doesn't contain enough space for packer\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	datas.n_entry = datas.v_addr + datas.c_offset + 256;
	printf("[+] Replacing entry points from %#lx to %#lx\n", datas.o_entry, datas.n_entry);

	// Encrypt .text zone
	datas.key = encrypt_zone((char *)(datas.f_map + datas.fs_offset), datas.fs_size);

	// Inject encryption key
	memmove(datas.f_map + datas.c_offset, datas.key, 256);
	printf("[+] Encryption key injected\n");

	// Inject depacker
	memmove(datas.f_map + datas.c_offset + 256, datas.p_map + datas.ps_offset, datas.ps_size);
	printf("[+] Packer code injected\n");

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
	uint64_t tmp_l = datas.fs_offset + datas.fs_size;
	tmp_p = tmp_p - (tmp_p % 4096);

	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x5555555555555555, tmp_p);
	printf("ASM update : mprotect zone start\n");

	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x6666666666666666, tmp_l);
	printf("ASM update : mprotect zone length\n");

	// Change entry point
	((Elf64_Ehdr *)(datas.f_map))->e_entry = datas.n_entry;

	// Unmap and save changes
	file_unmap(datas.f_map, datas.f_size);
	file_unmap(datas.p_map, datas.p_size);

	return (0);
}
