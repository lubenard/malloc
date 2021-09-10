/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:55:29 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/10 17:25:32 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <stddef.h>

# define PAGESIZE 4096
# define MAGIC_NUMBER 0xABCDEF

void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	*calloc(size_t nitems, size_t size);

typedef struct s_alloc {
	size_t size;
	short is_busy;
	struct s_alloc *next;
	struct s_alloc *prev;
}				t_alloc;

#endif
