EXEC		=	woody_woodpacker
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror
SRC			=	main \
				elf64_find_cave \
				elf64_find_sect \
				elf64_find_vaddr \
				elf64_is_valid \
				elf64_update_asm \
				encryption \
				file_copy \
				file_map \
				file_size \
				file_unmap

OBJ			=	$(addsuffix .o, $(SRC))

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -rf $(EXEC)

re: fclean all

.PHONY: all clean fclean re
