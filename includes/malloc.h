/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:55:29 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/06 17:15:35 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <stddef.h>
# include <stdint.h>

# define PAGESIZE 4096
# define MAGIC_NUMBER 0xABCDEF
# define TINY 1024
# define SMALL 2048
# define LARGE 4096

void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	*calloc(size_t nitems, size_t size);
void	show_alloc_mem();
void	show_alloc_mem_ex();

struct s_bloc {
	long				buffer_overflow;
	short			total_node;
	short			total_freed_node;
	int				total_size;
	struct s_bloc	*next;
	struct s_bloc	*prev;
}			__attribute__((packed));

typedef struct s_bloc t_bloc;

struct s_alloc {
	int		buffer_overflow;
	char	second_buffer;
	size_t	size;
	short	is_busy;
	struct	s_alloc *next;
	struct	s_alloc *prev;
}			__attribute__ ((packed));

typedef struct s_alloc t_alloc;

# define STRUCT_SIZE sizeof(t_alloc)

// Internal functions
void	split_node(t_alloc *node, size_t size_of_block);
void	*real_malloc(size_t size);
void	real_free(void *ptr);

#endif
