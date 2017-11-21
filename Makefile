EXEC		=	woody_woodpacker
CC			=	clang
CFLAGS		=	-Wall -Wextra -Werror
SRC			=	main \
				elf64_find_cave \
				elf64_find_sect \
				elf64_find_vaddr \
				elf64_update_vaddr \
				elf64_is_valid \
				elf64_update_asm \
				encryption \
				file_copy \
				file_map \
				file_size \
				file_unmap

OBJ			=	$(addsuffix .o, $(SRC))
LIB			=	./libft/libftprintf.a
LIBPATH		=	./libft/
LIBID		=	ftprintf
PACKER		=	packer
NASM		=	nasm
NASMFLAGS	=	-f elf64
ASMSRC		=	test_decryptage.s

$(EXEC): $(LIB) $(OBJ)
	$(info Compiling $(EXEC))
	@$(CC) -I$(LIBPATH) $(CFLAGS) -o $@ $^ -L$(LIBPATH) -l$(LIBID)
	$(NASM) $(NASMFLAGS) -o $(PACKER) $(ASMSRC)

all: $(EXEC)

$(LIB): 
	$(info Compiling libft)
	@make -C $(LIBPATH)
	@echo "Done !"

%.o: %.c
	$(info Compiling $< into $@ ...)
	@$(CC) $(CFLAGS) -o $@ -c $< -I$(LIBPATH)

clean:
	$(info Cleaning . and ./libft ...)
	@make $@ -C $(LIBPATH)
	rm -f $(OBJ)
	$(info Done !)

fclean: clean
	$(info Cleaning . and ./libft ...)
	@make $@ -C $(LIBPATH)
	@rm -rf $(EXEC)
	@rm -rf $(PACKER)
	$(info Done !)

re: fclean all

.PHONY: all clean fclean re
