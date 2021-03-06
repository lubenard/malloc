# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/08/26 14:06:44 by lubenard          #+#    #+#              #
#    Updated: 2021/10/05 18:11:14 by lubenard         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
    HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

SRCDIR = srcs

SRC_FILES_C = malloc.c \
			  free.c \
			  realloc.c \
			  calloc.c \
			  show_alloc_mem.c

CC = clang

SRCS_C = $(addprefix $(SRCDIR)/, $(SRC_FILES_C))

OBJ_C = $(SRCS_C:.c=.o)

CFLAGS = -Wall -Wextra -Werror -g3

all: $(NAME)

$(NAME): $(OBJ_C)
	@printf "\033[33mLinking of $(NAME)...\033[0m"
	@cd debug_lib && make
	@$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJ_C) debug_lib/libft_malloc_printf.a
	@printf "\033[32m[✓]\033[0m\n"
	@ln -sf libft_malloc_$(HOSTTYPE).so libft_malloc.so

%.o : %.c
	@printf "\033[36mCompilation de $<...\033[0m"
	@$(CC) -c $(CFLAGS) -fPIC $< -o $@ -Iincludes
	@printf "\033[32m[✓]\033[0m\n"

clean:
	@printf "\033[31mSuppression des *.o...\033[0m"
	@rm -rf $(OBJ_C)
	@printf "\033[32m[✓]\033[0m\n"

fclean: clean
	@printf "\033[31mSuppression de $(NAME)...\033[0m"
	@rm -f $(NAME)
	@rm -f libft_malloc.so
	#@rm -rf a.out a.out.dSYM
	@printf "\033[32m[✓]\033[0m\n"

re: fclean all

relaunch: fclean launch

test: re
	@clang -g3 main.c libft_malloc.so
	@./a.out

ltest: all
	@clang -g3 main.c libft_malloc.so
	@lldb ./a.out

vtest: re
	@clang -g3 main.c libft_malloc.so
	@valgrind ./a.out

.SILENT:

.PHONY: all re fclean clean
