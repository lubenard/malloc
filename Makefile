# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/08/26 14:06:44 by lubenard          #+#    #+#              #
#    Updated: 2021/09/02 19:07:39 by lubenard         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
    HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

SRCDIR = srcs

SRC_FILES_C = malloc.c

CC = clang

SRCS_C = $(addprefix $(SRCDIR)/, $(SRC_FILES_C))

OBJ_C = $(SRCS_C:.c=.o)

CFLAGS = -Wall -Wextra -Werror -g3 -fPIC

all: $(NAME)

$(NAME): $(OBJ_C)
	@printf "\033[33mLinking of $(NAME)...\033[0m"
	@ar rcs $(NAME) $(OBJ_C)
	@printf "\033[32m[✓]\033[0m\n"
	@ar rcs libft_malloc.dylib $(OBJ_C)
	@ln -s libft_malloc_$(HOSTTYPE).so libft_malloc.so

%.o : %.c
	@printf "\033[36mCompilation de $<...\033[0m"
	@clang -c $(CFLAGS) $< -o $@ -Iincludes
	@printf "\033[32m[✓]\033[0m\n"

clean:
	@printf "\033[31mSuppression des *.o...\033[0m"
	@rm -rf $(OBJ_C)
	@printf "\033[32m[✓]\033[0m\n"

fclean: clean
	@printf "\033[31mSuppression de $(NAME)...\033[0m"
	@rm -f $(NAME)
	@rm -f libft_malloc.so
	@rm -rf a.out a.out.dSYM
	@printf "\033[32m[✓]\033[0m\n"

re: fclean all

relaunch: fclean launch

test: re
	@clang main.c libft_malloc.so
	@./a.out

vtest: re
	@clang main.c libft_malloc.so
	@valgrind ./a.out

.SILENT:

.PHONY: all re fclean clean
