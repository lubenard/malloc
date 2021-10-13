/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:22:31 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/13 23:35:03 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/malloc.h"
#include "../debug_lib/srcs/memlib.h"
#include <pthread.h>

extern pthread_mutex_t g_mutex;
extern t_alloc *g_curr_node;

void	*realloc(void *ptr, size_t size) {
	void *ptr_realloc;
	t_alloc *node_ptr;
	size_t size_to_copy;

	pthread_mutex_lock(&g_mutex);
	if (ptr == NULL) {
		pthread_mutex_unlock(&g_mutex);
		return (real_malloc(size));
	}
	node_ptr = (t_alloc *)((char *) ptr - STRUCT_SIZE - 1);
	if (node_ptr->buffer_overflow == MAGIC_NUMBER && node_ptr->is_busy == 2) {
		size_to_copy = ((int)size < node_ptr->size) ? size : node_ptr->size - STRUCT_SIZE;
		ptr_realloc = real_malloc(size);
		ft_memcpy(ptr_realloc, ptr, size_to_copy);
		real_free(ptr);
	} else
		return NULL;
	pthread_mutex_unlock(&g_mutex);
	return ptr_realloc;
}
