/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:55:29 by lubenard          #+#    #+#             */
/*   Updated: 2021/08/26 15:32:39 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);

typedef struct s_alloc {
	void *data;
	short size;
	short size_remaining;
	short is_free;
	int magic_number;
	struct s_alloc *next;
}				t_alloc;

#endif
