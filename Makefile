# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rludosan <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/03/31 10:26:56 by rludosan          #+#    #+#              #
#    Updated: 2017/06/07 14:07:30 by rludosan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

EXEC		=	woody_woodpacker
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror
SRC			=	main\
				infos\
				open_fds\
				error_handler\
				check_elf\
				file_size\
				copy_file\
				mine_cave\
				packer_infect\
				encrypt\
				encryption
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
