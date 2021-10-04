/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:21:03 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/30 17:58:07 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <pthread.h>
extern pthread_mutex_t g_mutex;

// Debug
#include "../debug_lib/srcs/iolib.h"

// TODO: Rework this function
void merge_blocks(t_alloc *node_ptr) {
	t_alloc *node_tmp;

	node_tmp = node_ptr;
	printk("Merge block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		//printk("(Bloc merge) Reversing linked list : Actually on %p, going on %p\n", node_ptr, node_tmp->prev);
		node_tmp = node_tmp->prev;
	}
	printk("Current merge pointer is %p\n", node_tmp);
	while (node_tmp->next) {
		printk("(Bloc merge) Going on linked list : Actually on %p, going on %p\n", node_ptr, node_tmp->next);
		node_tmp = node_tmp->next;
	}
}

void	real_free(void *ptr) {
	t_alloc *node_ptr;

	if (ptr == 0)
		return;
	printk("---------REQUESTING FREE------------\n");
	printk("Getting %p from arg\n", ptr);
	node_ptr = (t_alloc *)((char *) ptr - sizeof(t_alloc) - 1);
	if (node_ptr->buffer_overflow == MAGIC_NUMBER) {
		printk("Freeing from address %p\n", node_ptr);
		node_ptr->is_busy = 1;
		/*if ((node_ptr->prev && node_ptr->prev->is_busy == 1) ||
			(node_ptr->next && node_ptr->next->is_busy == 1))
			merge_blocks(node_ptr);*/
		//munmap(node_ptr, sizeof(t_alloc) + node_ptr->size);
	}
	printk("----------END FREE---------------\n");
}

void free(void *ptr) {
	pthread_mutex_lock(&g_mutex);
	real_free(ptr);
	pthread_mutex_unlock(&g_mutex);
}
