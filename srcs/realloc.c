/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:22:31 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/28 18:44:09 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/malloc.h"
#include "../debug_lib/srcs/memlib.h"
#include <pthread.h>

extern pthread_mutex_t g_mutex;

// Debug
#include "../debug_lib/srcs/iolib.h"
void	*realloc(void *ptr, size_t size) {
	void *ptr_realloc;
	t_alloc *node_ptr;

	//pthread_mutex_lock(&g_mutex);
	printk("---REQUEST REALLOC with new size %lu from address %p-----\n", size, ptr);
	//pthread_mutex_unlock(&g_mutex);
	ptr_realloc = malloc(size);
	node_ptr = (t_alloc *)((char *) ptr - sizeof(t_alloc) - 1);
	if (ptr && node_ptr->buffer_overflow == MAGIC_NUMBER) {
		printk("Received pointer %p from arg\n", ptr);
		printk("Should copy %lu bytes, found on node %p\n", (int)node_ptr->size - (int)sizeof(t_alloc), node_ptr);
		printk("Starting at %p and finishing on %p\n", ptr, ((char*) ptr + node_ptr->size - sizeof(t_alloc)));
		ft_memcpy(ptr_realloc, ptr, ((t_alloc *)((char*) ptr - sizeof(t_alloc) - 1))->size - sizeof(t_alloc));
	}
	free(ptr);
	printk("----END REALLOC----\n");
	//pthread_mutex_unlock(&g_mutex);
	return ptr_realloc;
}

