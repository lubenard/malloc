/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:21:03 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/13 23:20:08 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <pthread.h>
#include <sys/mman.h>

extern pthread_mutex_t g_mutex;
extern t_alloc *g_curr_node;

// Debug
#include "../debug_lib/srcs/iolib.h"

void reorganize_pointer(t_block *node) {
	t_block  *cur_block;
	t_alloc *first_alloc_of_bloc;
	t_alloc *last_alloc_of_bloc;
	int i;

	i = 0;
	cur_block = node;
	//printk("Curr_block is %p, next is %p, prev is %p\n", cur_block, cur_block->next, cur_block->prev);
	//printk("Block is supposed to start on %p -> %p\n", cur_block, ((char *)cur_block + cur_block->total_size));
	first_alloc_of_bloc = (t_alloc *)((char *)cur_block + STRUCT_BLOCK_SIZE + 1);

	//printk("first_alloc_of_bloc is %p, next is %p, prev is %p\n", first_alloc_of_bloc, first_alloc_of_bloc->next, first_alloc_of_bloc->prev);

	/*if (first_alloc_of_bloc->buffer_overflow == MAGIC_NUMBER)
		printk("Integrity of alloc confirmed\n");
	else
		printk("alloc probably corrupt :(");
	*/

	//printk("%d / %d freed\n",cur_block->total_freed_node, cur_block->total_node);
	if (node->next) {
		last_alloc_of_bloc = ((t_alloc *)((char*)node->next + STRUCT_BLOCK_SIZE + 1))->prev;
		//printk("last_alloc_of_bloc is %p, next is %p, prev is %p\n", last_alloc_of_bloc, last_alloc_of_bloc->next, last_alloc_of_bloc->prev);
	} else
		last_alloc_of_bloc = 0;

	if (first_alloc_of_bloc && first_alloc_of_bloc->prev) {
		first_alloc_of_bloc->prev->next = (last_alloc_of_bloc) ? last_alloc_of_bloc->next : 0;
		//printk("first_alloc_of_bloc->prev (%p)->next = %p, was pointing on %p\n", first_alloc_of_bloc->prev, (last_alloc_of_bloc) ? last_alloc_of_bloc->next : 0, first_alloc_of_bloc);
	}
	if (last_alloc_of_bloc && last_alloc_of_bloc->next) {
		last_alloc_of_bloc->next->prev = first_alloc_of_bloc->prev;
		//printk("last_alloc_of_bloc->next (%p)->prev = %p, was pointing on %p\n", (last_alloc_of_bloc) ? last_alloc_of_bloc->next : 0, first_alloc_of_bloc->prev, last_alloc_of_bloc);
	}

	if (cur_block->prev) {
		cur_block->prev->next = cur_block->next;
		//printk("cur_block->prev (%p)->next = %p, was pointing on %p\n", cur_block->prev, cur_block->next, cur_block);
	}
	if (cur_block->next) {
		cur_block->next->prev = cur_block->prev;
		//printk("cur_block->next (%p)->prev = %p, was pointing on %p\n", cur_block->next, cur_block->prev, cur_block);
	}
}

int		check_real_freed_nodes(t_block *node) {
	t_alloc *first_alloc = (t_alloc *)((char*)node + STRUCT_BLOCK_SIZE + 1);
	int total_freed_node = 0;
	int total_node = 0;

	while (first_alloc->block == node) {
		if (first_alloc->block != node) {
			//printk("/!\\ Bad redirection detected !!!!! first_alloc = %p, first_alloc->prev = %p\n", first_alloc, first_alloc->prev);
			break;
		}
		if (first_alloc->is_busy == ALLOC_FREE)
			total_freed_node++;
		total_node++;
		first_alloc = first_alloc->next;
	}

	//printk("Real total nodes for bloc %p is %d freed on %d\n", node, total_freed_node, total_node);
	if (total_node == 0)
		total_node = 1;
	node->total_node = total_node;
	node->total_freed_node = total_freed_node;
	return total_freed_node;
}

void	check_block_to_free(t_alloc *alloc) {
	t_block *block_tmp;

	block_tmp = alloc->block;

	//printk("(check_block_to_free) curr_block is %p, next is %p, prev is %p\n", block_tmp, block_tmp->next, block_tmp->prev);
	while (block_tmp->prev)
		block_tmp = block_tmp->prev;

	//printk("Curr_block is %p, next is %p, prev is %p\n", block_tmp, block_tmp->next, block_tmp->prev);
	while (block_tmp) {
		if (g_curr_node->block == block_tmp) {
			//printk("Move on, crashy condition\n");
			block_tmp = block_tmp->next;
			continue;
		}
		if (check_real_freed_nodes(block_tmp) == block_tmp->total_node) {
			check_real_freed_nodes(block_tmp);
			printk("Should launch munmap for block %p, size %lu\n", block_tmp, block_tmp->total_size);
			reorganize_pointer(block_tmp);
			munmap(block_tmp, block_tmp->total_size + 1);
			return;
		} //else
			//printk("check_block_to_free node %p : %d/%d freed\n", block_tmp, block_tmp->total_freed_node, block_tmp->total_node);
		block_tmp = block_tmp->next;
	}
}

void	real_free(void *ptr) {
	t_alloc *node_ptr;

	//printk("---------REQUESTING FREE------------\n");
	if (ptr == 0)
		return;
	//printk("Getting %p from arg\n", ptr);
	node_ptr = (t_alloc *)((char *) ptr - STRUCT_SIZE - 1);
	if (node_ptr->buffer_overflow == MAGIC_NUMBER && node_ptr->is_busy != ALLOC_FREE) {
		//printk("Freeing from address %p\n", node_ptr);
		node_ptr->is_busy = ALLOC_FREE;
		if (node_ptr->block->total_node == 0)
			node_ptr->block->total_node = 1;
		if (node_ptr->block->next || node_ptr->block->prev) {
			check_block_to_free(node_ptr);
		}
	} else
		printk("Invalid free\n");
	//printk("----------END FREE---------------\n");
}

void free(void *ptr) {
	pthread_mutex_lock(&g_mutex);
	real_free(ptr);
	pthread_mutex_unlock(&g_mutex);
}
