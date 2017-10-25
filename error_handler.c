#include "woody_woodpacker.h"

int		error_handler(int code)
{
	if (code == E_USAGE)
		printf("Usage: ./woody_woodpacker <src> <dst>\n");
	else if (code == E_FDOPEN)
		printf("Error: unable to open source and/or destination file(s)\n");
	else if (code == E_ELFHDR)
		printf("Error: unable to read elf64 header\n");
	else if (code == E_FSIZE)
		printf("Error: unable to get file size\n");
	else if (code == E_COPY)
		printf("Error: unable to copy binary file\n");
	else if (code == E_MCAVE)
		printf("Error: unable to find a code cave\n");
	else if (code == E_INFECT)
		printf("Error: unable to infect target file\n");
	else
		printf("Error: ???\n");
	return (-1);
}
