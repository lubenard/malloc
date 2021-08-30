/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:55:29 by lubenard          #+#    #+#             */
/*   Updated: 2021/08/30 21:04:26 by lubenard         ###   ########.fr       */
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

typedef struct s_alloc {
	size_t size_remaining;
	short is_free;
	struct s_alloc *next;
	struct s_alloc *prev;
}				t_alloc;

#endif
