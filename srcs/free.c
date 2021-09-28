/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:21:03 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/28 11:56:08 by lubenard         ###   ########.fr       */
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
	t_alloc *node_tmp_2;

	node_tmp = node_ptr;
	printk("Merge block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		//printk("Reversing linked list : Actually on %p, going on %p\n", node_ptr, node_tmp->prev);
		node_tmp = node_tmp->prev;
	}
	printk("Current merge pointer is %p\n", node_tmp);
	while (node_tmp->next) {
		if (node_tmp->is_busy == 0) {
			break;
		}
		node_tmp = node_tmp->next;
	}
	node_tmp_2 = node_tmp;
	node_tmp = node_tmp->next;
	while (node_tmp->next) {
		if (node_tmp->is_busy) {
			node_tmp_2->size += node_tmp->size;
			printk("Merging %p with %p, for total size of %lu\n", node_tmp_2, node_tmp, node_tmp_2->size);
		}
		node_tmp = node_tmp->next;
	}
}

void	free(void *ptr) {
	t_alloc *node_ptr;

	if (ptr == 0)
		return;
	pthread_mutex_lock(&g_mutex);
	printk("---------REQUESTING FREE------------\n");
	printk("Getting %p from arg\n", ptr);
	//printk("Pointer should be at %p\n", g_curr_node->prev);
	node_ptr = (t_alloc *)((char *) ptr - sizeof(t_alloc) - 1);
	printk("Freeing from address %p\n", node_ptr);
	node_ptr->is_busy = 1;
	//merge_blocks(node_ptr);
	//munmap(node_ptr, sizeof(t_alloc) + node_ptr->size);
	printk("----------END FREE---------------\n");
	pthread_mutex_unlock(&g_mutex);
}
