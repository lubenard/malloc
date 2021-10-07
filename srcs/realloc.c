/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:22:31 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/05 19:29:40 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/malloc.h"
#include "../debug_lib/srcs/memlib.h"
#include <pthread.h>

extern pthread_mutex_t g_mutex;
extern t_alloc *g_curr_node;

// Debug
#include "../debug_lib/srcs/iolib.h"

void resplit_node(t_alloc *start_node, t_alloc *end_node) {
	printk("Merging %p with %p\n", start_node, end_node);
	printk("Old size of start_node is %lu, and end_node is %lu\n", start_node->size, end_node->size);
	start_node->size += end_node->size;
	printk("New size of start_node is %lu\n", start_node->size);
	start_node->next = end_node->next;
}

void	*realloc(void *ptr, size_t size) {
	void *ptr_realloc;
	t_alloc *node_ptr;
	size_t size_to_copy;

	pthread_mutex_lock(&g_mutex);
	printk("---REQUEST REALLOC with new size %lu from address %p-----\n", size, ptr);
	if (ptr == NULL) {
		printk("Null ptr, return real malloc\n");
		pthread_mutex_unlock(&g_mutex);
		return (real_malloc(size));
	}
	node_ptr = (t_alloc *)((char *) ptr - STRUCT_SIZE - 1);
	if (node_ptr->buffer_overflow == MAGIC_NUMBER
		&& node_ptr->next && node_ptr->next->is_busy == 1
		&& (size_t)(node_ptr->size - STRUCT_SIZE) + (size_t)(node_ptr->next->size - STRUCT_SIZE) > size) {
		printk("Compute is %lu + (next) %lu > %lu\n", (size_t)(node_ptr->size - STRUCT_SIZE), (size_t)(node_ptr->next->size - STRUCT_SIZE), size);
		printk("Block at %p has %lu bytes availables and is_busy is 1\n", node_ptr->next, node_ptr->next->size - STRUCT_SIZE);
		printk("Block %p and %p can contain (%lu + %lu ) > %lu\n", node_ptr, node_ptr->next, node_ptr->size - STRUCT_SIZE, node_ptr->next->size - STRUCT_SIZE, size);
		printk("Splitting node in realloc at node %p with size %lu\n", node_ptr, size);
		resplit_node(node_ptr, node_ptr->next);
		split_node(node_ptr, size);
		ptr_realloc = ptr;
	} /*else if (node_ptr->prev && node_ptr->prev->is_busy == 1
		&& (size_t)(node_ptr->size - STRUCT_SIZE) + (size_t)(node_ptr->prev->size - STRUCT_SIZE) > size) {
		printk("Compute is %lu + (prev) %lu > %lu\n", (size_t)(node_ptr->size - STRUCT_SIZE), (size_t)(node_ptr->prev->size - STRUCT_SIZE), size);
		printk("Block at %p has %lu bytes availables and is_busy is 1\n", node_ptr->prev, node_ptr->prev->size - STRUCT_SIZE);
		printk("Block %p and %p can contain (%lu + %lu ) > %lu\n", node_ptr, node_ptr->prev, node_ptr->size - STRUCT_SIZE, node_ptr->prev->size - STRUCT_SIZE, size);
		printk("Splitting node in realloc at node %p with size %lu\n", node_ptr, size);
		resplit_node(node_ptr->prev, node_ptr);
		split_node(node_ptr->prev, size);
		ptr_realloc = ((char*) node_ptr->prev + STRUCT_SIZE + 1);
	}*/ else {
		size_to_copy = ((int)size < node_ptr->size) ? size : node_ptr->size - STRUCT_SIZE;
		ptr_realloc = real_malloc(size);
		ft_memcpy(ptr_realloc, ptr, size_to_copy);
		real_free(ptr);
	}
	printk("----END REALLOC, return %p----\n", ptr_realloc);
	pthread_mutex_unlock(&g_mutex);
	return ptr_realloc;
}
