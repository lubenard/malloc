/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/06 17:15:12 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include "malloc.h"

//TODO: Used for ft_memcpy, clean it later
#include "../debug_lib/srcs/memlib.h"
t_alloc *g_curr_node = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

// DEBUG ONLY
#include <unistd.h>
#include "../debug_lib/srcs/iolib.h"
#include "../debug_lib/srcs/strlib.h"

t_alloc *curr_block_start;
t_alloc *curr_block_end;

// END DEBUG ONLY
size_t roundUp(void *a, size_t b) {
	return (1 + ((size_t)a - 1) / b) * b;
}

size_t roundUpDiff(void *a, size_t b) {
	return (size_t)roundUp(a, b) - (size_t)a;
}

t_alloc *init_node(size_t size_requested) {
	t_bloc *bloc;
	t_alloc *node;

	printk("Creating NEW node: %lu bytes long (%lu + %lu) <- STRUCT_SIZE\n", size_requested + STRUCT_SIZE, size_requested, STRUCT_SIZE);
	size_requested += STRUCT_SIZE;
	size_requested = (size_requested / PAGESIZE + 1) * PAGESIZE;
	printk("Creating NEW node: %lu bytes long\n", size_requested);
	bloc = mmap(NULL, size_requested, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	node = (t_alloc *)((char *)bloc + sizeof(t_bloc));
	printk("Bloc is stored at %p, and node at %p (+%lu)\n", bloc, node, sizeof(t_bloc));
	if (!bloc || bloc == MAP_FAILED) {
		printk("/!\\mmap failed\n");
		return 0;
	}

	bloc->total_size = size_requested;
	printk("Node begin at %p and end at %p\n", node, (t_alloc *)((char *)node + size_requested));
	curr_block_start = node;
	curr_block_end = (t_alloc *)((char *)node + size_requested);
	printk("Node pointer is %p (size - STRUCT_SIZE)\n", node);
	node->size = size_requested - sizeof(t_bloc);
	printk("Node->size is %lu\n", node->size);
	node->buffer_overflow = MAGIC_NUMBER;
	// Available: 1, Not Available: 2
	node->is_busy = 1;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

short	create_link_new_node(size_t size_of_block) {
	t_alloc *node;

	if (!(node = init_node(size_of_block)))
		return 0;
	// Link only if g_curr_node exist
	if (g_curr_node) {
		node->prev = g_curr_node;
		g_curr_node->next = node;
	}
	g_curr_node = node;
	printk("Placed g_curr_node @ %p\n", g_curr_node);
	return 1;
}

void split_node(t_alloc *node, size_t size_of_block) {
	t_alloc *new_node;

	size_t old_size_block = node->size;

	new_node = (t_alloc *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16);

	printk("Could we place struct @ %p, cause %p (+32) is aligned\n", roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16), ((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE + 1));

	if ((uintptr_t)((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE + 1) % 16 == 0)
		printk("Check, pointer %p is aligned\n", ((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE + 1));
	else
		printk("check, pointer %p not aligned\n", ((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE));

	node->size = (size_t)new_node - (size_t)node;
	new_node->size = old_size_block - node->size;
	printk("New_node->size %lu - %lu (%lu + %lu + %lu + 1) = %lu\n", old_size_block, node->size, STRUCT_SIZE, size_of_block, roundUpDiff(((char *)node + STRUCT_SIZE + size_of_block + 1), 16), new_node->size);
	printk("Node->size (%p) is %lu, and new_node->size (%p) = %lu\n", node, node->size, new_node, new_node->size);
	printk("Node %p is marqued as not available\n", node);
	node->is_busy = 2;
	new_node->is_busy = 1;
	new_node->buffer_overflow = MAGIC_NUMBER;
	new_node->next = 0;
	node->next = new_node;
	new_node->prev = node;
	printk("Node->next = %p, node->prev = %p\n", node->next, node->prev);
	printk("node->buffer_overflow = %d\n", node->buffer_overflow);
	printk("New_node->buffer_overflow = %d\n", new_node->buffer_overflow);
	g_curr_node = new_node;
	printk("Placed g_curr_node @ %p\n", g_curr_node);
}

t_alloc		*find_place_at_beginning(size_t size_looked) {
	t_alloc *node_tmp;

	node_tmp = g_curr_node;
	printk("find_place_at_beginning block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		//printk("Reversing linked list : Actually on %p, going on %p\n", node_tmp, node_tmp->prev);
		if (node_tmp->prev->buffer_overflow != MAGIC_NUMBER) {
			printk("Probably node corruption on %p\n", node_tmp->prev);
			//return 0;
		}
		if (node_tmp->is_busy == 1 && node_tmp->size > size_looked) {
			printk("(find_place_at_beginning) Found space for %lu at %p (%lu bytes available in the node)\n", size_looked, node_tmp, node_tmp->size);
			return node_tmp;
		}
		node_tmp = node_tmp->prev;
	}
	printk("find_place_at_beginning No node able to contain it found\n");
	return 0;
}

t_alloc *should_split(size_t tmp_value, size_t size) {
	t_alloc *return_node_ptr;

	printk("Split ? g_curr_node at addr (%p) size %lu - %lu = %lu > 0 = %s\n", g_curr_node, g_curr_node->size, tmp_value + STRUCT_SIZE, (int)g_curr_node->size - (int)tmp_value - (int)STRUCT_SIZE, ((int)g_curr_node->size - (int)tmp_value - (int)STRUCT_SIZE > 0) ? "YES" : "NO");
	// Split node if needed
	if ((int)g_curr_node->size - (int)tmp_value - (int)STRUCT_SIZE > 0) {
		return_node_ptr = g_curr_node;
		split_node(g_curr_node, size);
	} else {
		printk("Node %p is marqued as not available\n", g_curr_node);
		printk("Node %p has size %lu\n", g_curr_node, g_curr_node->size);
		g_curr_node->is_busy = 2;
		return_node_ptr = g_curr_node;
	}
	return return_node_ptr;
}

void	*real_malloc(size_t size) {
	t_alloc *return_node_ptr = 0;
	t_alloc *tmp_g_curr_node = 0;
	t_alloc *tmp2_g_curr_node = 0;

	printk("int %d, short %d, long %d\n", sizeof(int), sizeof(short), sizeof(long));

	printk("-------REQUESTING NEW MALLOC OF SIZE %lu---------\n", size);
	if (size == 0)
		return NULL;
	if (!g_curr_node) {
		if (!create_link_new_node(size))
			return NULL;
		printk("Head of linked list is now init @ %p\n", g_curr_node);
	}
	printk("g_curr_node = %p with size %lu\n", g_curr_node, g_curr_node->size);
	size_t tmp_value = STRUCT_SIZE + size + 1;
	if (g_curr_node->is_busy == 2 || tmp_value > g_curr_node->size) {

		if (g_curr_node->is_busy == 2)
			printk("Creating new node cause g_curr_node is set as locked\n");
		else
			printk("Size (%lu) > g_curr_node->size (%lu)\n", tmp_value, g_curr_node->size);

		if (!(tmp_g_curr_node = find_place_at_beginning(tmp_value))) {
			if (!create_link_new_node(size))
				return NULL;
		} else {
			tmp2_g_curr_node = g_curr_node;
			g_curr_node = tmp_g_curr_node;
		}

		return_node_ptr = should_split(tmp_value, size);
	} else {
		if (g_curr_node->buffer_overflow == MAGIC_NUMBER) {
			printk("Found space for %lu (%lu + %lu) bytes in block located at %p (%lu bytes available)\n", tmp_value, size, STRUCT_SIZE, g_curr_node, g_curr_node->size);
			return_node_ptr = should_split(tmp_value, size);
		} else {
			printk("Possibly corrupted node on %p, magic number is %d\n", g_curr_node, g_curr_node->buffer_overflow);
			return NULL;
		}
	}
	printk("Block begin at %p and end at %p\n", curr_block_start, curr_block_end);
	printk("Node begin at %p and end at %p\n", return_node_ptr, ((char *)return_node_ptr + STRUCT_SIZE));
	printk("Node check before return : prev -> %p and next -> %p\n", return_node_ptr->prev, return_node_ptr->next);
	printk("Returning %p with size %lu (real size %lu) from malloc call. Original ptr is %p\n", ((char *)return_node_ptr + STRUCT_SIZE + 1), return_node_ptr->size - STRUCT_SIZE,return_node_ptr->size, return_node_ptr);
	if (tmp2_g_curr_node) {
		g_curr_node = tmp2_g_curr_node;
		tmp2_g_curr_node = 0;
	}
	printk("Final check before launching, is pointer aligned ? %s\n",
			(((uintptr_t)((char *)return_node_ptr + STRUCT_SIZE + 1) % 16) == 0) ? "YES" : "NO");
	printk("~~~~~~~END MALLOC~~~~~~~~~\n");
	return ((char *)return_node_ptr + STRUCT_SIZE + 1);
}

void	*malloc(size_t size) {
	void *return_ptr;

	pthread_mutex_lock(&g_mutex);
	return_ptr = real_malloc(size);
	pthread_mutex_unlock(&g_mutex);
	return return_ptr;
}
