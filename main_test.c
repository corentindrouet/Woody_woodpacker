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

	// Check arguments
	if (argc != 2)
	{
		printf("Usage: %s <binary>\n", argv[0]);
		return (-1);
	}

	// Mmap packer file
	if (file_map(PACKER_FILE, &(datas.p_map), &(datas.p_size), 0) == -1)
	{
		printf("Error: Mmap failed on %s\n", PACKER_FILE);
		return (-1);
	}

	// Check if packer file is a REL file
	if (elf64_is_rel((Elf64_Ehdr *)datas.p_map) == -1)
	{
		printf("Error: %s is not a valid rel file\n", PACKER_FILE);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	// Find packer file .text section
	if (elf64_find_sect(datas.p_map, &(datas.ps_offset), &(datas.ps_size), ".text") == -1)
	{
		printf("Error: %s .text section not found\n", TARGET_FILE);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}
	printf("[+] Infectious code .text section found at offset %lu (%lu bytes)\n", datas.ps_offset, datas.ps_size);

	// Mmap source file
	if (file_map(argv[1], &(datas.f_map), &(datas.f_size), 0) == -1)
	{
		printf("Error: Mmap failed on %s\n", argv[1]);
		return (-1);
	}

	// Check if source file is an executable
	if (elf64_is_exec((Elf64_Ehdr *)datas.f_map) == 0 || elf64_is_dyn((Elf64_Ehdr *)datas.f_map) == 0)
	{
		//
	}
	else
	{
		printf("Error: '%s' is not an exec file\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		return (-1);
	}

	// Save source file entry point
	datas.o_entry = ((Elf64_Ehdr *)(datas.f_map))->e_entry;
	printf("[+] Actual target file entry point is 0x%lx\n", datas.o_entry);

	// Find source file .text section
	if (elf64_find_sect(datas.f_map, &(datas.fs_offset), &(datas.fs_size), ".text") == -1)
	{
		printf("Error: %s .text section not found\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		return (-1);
	}
	printf("[+] Target file .text section found at offset %lu (%lu bytes)\n", datas.fs_offset, datas.fs_size);

	// Find source file PT_LOAD executable segment
	if (elf64_find_vaddr(datas.f_map, &(datas.v_addr), datas.ps_size + 256, &(datas.c_offset), &(datas.c_size)) == -1)
	{
		printf("Error: %s doesn't contain enough space for packer\n", TARGET_FILE);
		file_unmap(datas.f_map, datas.f_size);
		return (-1);
	}

	// Save source file new entry point
	datas.n_entry = datas.v_addr + datas.c_offset + 256;
	printf("[+] Updated target file entry point is 0x%lx\n", datas.n_entry);

	// Release source file
	file_unmap(datas.f_map, datas.f_size);

	// Copy source file into destination file (bin -> woody)
	if (file_copy(argv[1], TARGET_FILE) == -1)
	{
		printf("Error: File copy failed\n");
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	// Mmap destination file
	if (file_map(TARGET_FILE, &(datas.f_map), &(datas.f_size), 1) == -1)
	{
		printf("Error: Mmap failed on %s\n", TARGET_FILE);
		file_unmap(datas.p_map, datas.p_size);
		return (-1);
	}

	// Inject code
	printf("Injecting depacker code ...\n");
	memmove(datas.f_map + datas.c_offset + 256, datas.p_map + datas.ps_offset, datas.ps_size);
	printf("[+] Update ASM code for code jump\n");
	elf64_update_asm(datas.f_map + datas.c_offset + 256, datas.ps_size, 0x1111111111111111, (uint64_t)(datas.o_entry));

	printf("Updating entry point ...\n");
	((Elf64_Ehdr *)(datas.f_map))->e_entry = datas.n_entry;

	file_unmap(datas.f_map, datas.f_size);
	file_unmap(datas.p_map, datas.p_size);

	printf("Encryption done !\n");

	return (0);
}