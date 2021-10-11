/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:21:03 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/11 01:59:31 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <pthread.h>
#include <sys/mman.h>
extern pthread_mutex_t g_mutex;

// Debug
#include "../debug_lib/srcs/iolib.h"

// TODO: Rework this function
void merge_blocks(t_alloc *node_ptr) {
	t_alloc *node_tmp;

	node_tmp = node_ptr;
	//printk("Merge block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		////printk("(Bloc merge) Reversing linked list : Actually on %p, going on %p\n", node_ptr, node_tmp->prev);
		node_tmp = node_tmp->prev;
	}
	//printk("Current merge pointer is %p\n", node_tmp);
	while (node_tmp->next) {
		//printk("(Bloc merge) Going on linked list : Actually on %p, going on %p\n", node_ptr, node_tmp->next);
		node_tmp = node_tmp->next;
	}
}

int		get_block_count(t_bloc *block) {
	t_bloc *tmp_block;
	int i = 1;

	tmp_block = block;
	if (block->prev != NULL) {
		while (tmp_block->prev) {
			i++;
			tmp_block = tmp_block->prev;
		}
	}

	if (block->next != NULL) {
		tmp_block = block;
		while (tmp_block->next) {
			i++;
			tmp_block = tmp_block->next;
		}
	}
	return i;
}

void reorganize_pointer(t_bloc *node) {
	t_bloc  *cur_block;
	t_alloc *first_alloc_of_bloc;
	t_alloc *last_alloc_of_bloc;
	int i;

	i = 0;
	cur_block = node;
	printk("Curr_block is %p, next is %p, prev is %p\n", cur_block, cur_block->next, cur_block->prev);
		first_alloc_of_bloc = (t_alloc *)((char *)cur_block + sizeof(t_bloc) + 1);

	printk("first_alloc_of_bloc is %p, next is %p, prev is %p\n", first_alloc_of_bloc, first_alloc_of_bloc->next, first_alloc_of_bloc->prev);

	if (first_alloc_of_bloc->buffer_overflow == MAGIC_NUMBER)
		printk("Integrity of alloc confirmed\n");
	else
		printk("alloc probably corrupt :(");

	printk("Block is supposed to start on %p -> %p\n", cur_block, ((char *)cur_block + cur_block->total_size));

	printk("%d / %d freed\n",cur_block->total_freed_node, cur_block->total_node);
	last_alloc_of_bloc = first_alloc_of_bloc;
	while (i != cur_block->total_node) {
		printk("%d / %d : last_alloc_of_bloc is %p (with size %lu), prev is %p, next is %p\n",i + 1, cur_block->total_node, last_alloc_of_bloc, last_alloc_of_bloc->size, last_alloc_of_bloc->prev, last_alloc_of_bloc->next);
		last_alloc_of_bloc = last_alloc_of_bloc->next;
		i++;
	}

	printk("last_alloc_of_bloc is %p, next is %p, prev is %p\n", last_alloc_of_bloc, last_alloc_of_bloc->next, last_alloc_of_bloc->prev);

	if (first_alloc_of_bloc->prev) {
		first_alloc_of_bloc->prev->next = last_alloc_of_bloc->next;
		printk("Redir made 2, %p now point on %p, was pointing on %p\n", first_alloc_of_bloc->prev, last_alloc_of_bloc->next, first_alloc_of_bloc);
	}
	if (last_alloc_of_bloc->next) {
		last_alloc_of_bloc->next->prev = first_alloc_of_bloc->prev;
		printk("Redir made 2, %p now point on %p, was pointing on %p\n", last_alloc_of_bloc->next, first_alloc_of_bloc->prev, last_alloc_of_bloc);
	}

	if (cur_block->prev) {
		cur_block->prev->next = cur_block->next;
		printk("Block redir made, %p now point on %p, was pointing on %p\n", cur_block->prev, cur_block->next, cur_block);
	}
	if (cur_block->next) {
		cur_block->next->prev = cur_block->prev;
		printk("Block redir made, %p now point on %p, was pointing on %p\n", cur_block->next, cur_block->prev, cur_block);
	}

}

void	check_block_to_free(t_alloc *alloc) {
	t_bloc *block_tmp;

	block_tmp = alloc->block;

	printk("Curr_block is %p, next is %p, prev is %p\n", block_tmp, block_tmp->next, block_tmp->prev);
	if (block_tmp->prev) {
		block_tmp = block_tmp->prev;
		printk("Curr_block is %p, next is %p, prev is %p\n", block_tmp, block_tmp->next, block_tmp->prev);
		while (block_tmp) {
			//printk("Entering the loop, actually on %p\n", block_tmp);
			if (block_tmp->total_freed_node == block_tmp->total_node) {
				printk("Should launch munmap for block %p and size %lu\n", block_tmp, block_tmp->total_size);
				reorganize_pointer(block_tmp);
				printk("Munmap for bloc %p and size %d\n", block_tmp, block_tmp->total_size);
				munmap(block_tmp, block_tmp->total_size);
				return;
			} //else
				//printk("check_block_to_free node %p : %d/%d freed\n", block_tmp, block_tmp->total_freed_node, block_tmp->total_node);
			block_tmp = block_tmp->prev;
		}
	}
}

void	real_free(void *ptr) {
	t_alloc *node_ptr;

	if (ptr == 0)
		return;
	printk("---------REQUESTING FREE------------\n");
	//printk("Getting %p from arg\n", ptr);
	node_ptr = (t_alloc *)((char *) ptr - STRUCT_SIZE - 1);
	if (node_ptr->buffer_overflow == MAGIC_NUMBER) {
		printk("Freeing from address %p\n", node_ptr);
		node_ptr->is_busy = 1;
		if (node_ptr->block->total_freed_node < node_ptr->block->total_node)
			node_ptr->block->total_freed_node++;
		printk("Incrementing total_freed_node of block %p, now %lu/%lu\n",node_ptr->block ,node_ptr->block->total_freed_node, node_ptr->block->total_node);
		/*if ((node_ptr->prev && node_ptr->prev->is_busy == 1) ||
			(node_ptr->next && node_ptr->next->is_busy == 1))
			merge_blocks(node_ptr);*/
		if (node_ptr->block->total_node == 0)
			node_ptr->block->total_node = 1;
		printk("get_block_count return %d\n", get_block_count(node_ptr->block));
		if (get_block_count(node_ptr->block) > 1) {
			check_block_to_free(node_ptr);
			////printk("Should launch munmap for block %p and size %lu\n", node_ptr->block, node_ptr->block->total_size);
		}
	}
	printk("----------END FREE---------------\n");
}

void free(void *ptr) {
	pthread_mutex_lock(&g_mutex);
	real_free(ptr);
	pthread_mutex_unlock(&g_mutex);
}
