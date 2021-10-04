/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:22:31 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/04 14:01:02 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/malloc.h"
#include "../debug_lib/srcs/memlib.h"
#include <pthread.h>

extern pthread_mutex_t g_mutex;
extern t_alloc *g_curr_node;

// Debug
#include "../debug_lib/srcs/iolib.h"


void	*ft_realloc(void *ptr, size_t size) {
	void *ptr_realloc;
	t_alloc *node_ptr;
	size_t size_to_copy;

	//pthread_mutex_lock(&g_mutex);
	//printk("---REQUEST REALLOC with new size %lu from address %p-----\n", size, ptr);
	//pthread_mutex_unlock(&g_mutex);
	ptr_realloc = malloc(size);
	node_ptr = (t_alloc *)((char *) ptr - sizeof(t_alloc) - 1);
	if (ptr && node_ptr->buffer_overflow == MAGIC_NUMBER) {
		if (size < node_ptr->size)
			size_to_copy = size;
		else
			size_to_copy = node_ptr->size - sizeof(t_alloc);
		//printk("Received pointer %p from arg\n", ptr);
		//printk("Should copy %lu bytes, but actually copying %lu, found on node %p\n", node_ptr->size - sizeof(t_alloc), size_to_copy, node_ptr);
		//printk("Starting at %p and finishing on %p\n", ptr, ((char*) ptr + node_ptr->size - sizeof(t_alloc)));
		ft_memcpy(ptr_realloc, ptr, size_to_copy);
		//printk("Finished copying\n");
	}
	free(ptr);
	//printk("----END REALLOC----\n");
	//pthread_mutex_unlock(&g_mutex);
	return ptr_realloc;
}

void	*realloc(void *ptr, size_t size) {
	//void *ptr_realloc;
	t_alloc *node_ptr;

	pthread_mutex_lock(&g_mutex);
	if (ptr == NULL)
		return (real_malloc(size));
	printk("---REQUEST REALLOC with new size %lu from address %p-----\n", size, ptr);
	node_ptr = (t_alloc *)((char *) ptr - sizeof(t_alloc) - 1);
	if (node_ptr->next && node_ptr->next->is_busy == 1) {
		if ((size_t)(node_ptr->size - sizeof(t_alloc)) + (size_t)(node_ptr->next - sizeof(t_alloc)) > size) {
			printk("Splitting node in realloc at node %p with size\n", node_ptr, size);
			split_node(node_ptr, size);
		}
	}
	pthread_mutex_unlock(&g_mutex);
	printk("----END REALLOC----\n");
	return ft_realloc(ptr, size);
}

