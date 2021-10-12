/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/12 03:23:45 by lubenard         ###   ########.fr       */
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

t_bloc *curr_block_start;
t_bloc *curr_block_end;

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

	////printk("Creating NEW node: %lu bytes long (%lu + %lu) <- STRUCT_SIZE\n", size_requested + STRUCT_SIZE, size_requested, STRUCT_SIZE);
	size_requested += STRUCT_SIZE + sizeof(t_bloc) + 1;
	size_requested = (size_requested / PAGESIZE + 1) * PAGESIZE;
	////printk("Creating NEW node: %lu bytes long\n", size_requested);
	bloc = mmap(NULL, size_requested, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	node = (t_alloc *)((char *)bloc + sizeof(t_bloc) + 1);
	////printk("Bloc is stored at %p, and node at %p (+%lu + 1)\n", bloc, node, sizeof(t_bloc));
	if (!bloc || bloc == MAP_FAILED) {
		//////printk("/!\\mmap failed\n");
		return 0;
	}


	bloc->total_node = 0;
	bloc->total_freed_node = 0;
	bloc->total_size = size_requested - 1;
	//printk("Create new bloc start at %p, and end at %p (size %lu)\n", bloc, ((char *)bloc + bloc->total_size), bloc->total_size + 1);
	curr_block_start = bloc;
	curr_block_end = (t_bloc *)((char *)bloc + bloc->total_size);
	//////printk("Node pointer is %p (size - STRUCT_SIZE)\n", node);
	node->size = bloc->total_size - sizeof(t_alloc) - 1;
	//////printk("Node->size is %lu\n", node->size);
	node->buffer_overflow = MAGIC_NUMBER;
	// Available: 1, Not Available: 2
	node->is_busy = ALLOC_FREE;
	node->next = NULL;
	//////printk("node->block point on %p\n", bloc);
	node->block = bloc;
	node->prev = NULL;

	bloc->next = NULL;
	bloc->prev = (g_curr_node) ? g_curr_node->block : NULL;
	if (g_curr_node) {
		printk("g_curr_node is %p, prev is %p, next is %p\n", g_curr_node, g_curr_node->prev, g_curr_node->next);
		g_curr_node->block->next = bloc;
		////printk("Bloc->prev = %p, because g_curr_node = %p and g_curr_node->block = %p\n", bloc->prev, g_curr_node, g_curr_node->block);
	}
	printk("bloc->next = %p, bloc->prev = %p\n", bloc->next, bloc->prev);
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
	//////printk("Placed g_curr_node @ %p\n", g_curr_node);
	return 1;
}

t_alloc *split_node(t_alloc *node, size_t size_of_block) {
	t_alloc *new_node;

	size_t old_size_block = node->size;

	new_node = (t_alloc *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16);

	////printk("roundUpDiff return %lu from address %p\n", roundUpDiff(((char *)node + STRUCT_SIZE + size_of_block + 1), 16), ((char *)node + STRUCT_SIZE + size_of_block + 1));

	////printk("Could we place struct @ %p, cause %p (+32) is aligned\n", roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16), ((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE + 1));

	//if ((uintptr_t)((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE + 1) % 16 == 0)
		//////printk("Check, pointer %p is aligned\n", ((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE + 1));
	//else
		//////printk("check, pointer %p not aligned\n", ((char *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16) + STRUCT_SIZE));

	node->size = (size_t)new_node - (size_t)node - 1;
	////printk("Size of block is %lu, node begin at %p, datas start at %p, datas should end at %p, node end at %p\n", size_of_block, node, ((char *)node + STRUCT_SIZE + 1), ((char *)node + STRUCT_SIZE + size_of_block), ((char*) node + node->size));
	//node->size = STRUCT_SIZE + size_of_block + roundUpDiff(((char *)node + STRUCT_SIZE + size_of_block + 1), 16);
	//////printk("Node %p is marqued as not available\n", node);
	//////printk("node->size %lu - %lu = %lu\n", (size_t)new_node, (size_t)node, (size_t)new_node - (size_t)node - 1);
	////printk("check node->size = %lu\n", node->size);
	node->block->total_node++;
	////printk("Incrementing total node for pointer %p, bringing it to %d\n", node->block, node->block->total_node);
	node->is_busy = ALLOC_USED;

	////printk("Test compute new_alloc_size = %lu\n", ((char *)node->block + node->block->total_size) - ((char *)new_node));
	//////printk("Test compute2 new_alloc_size = %lu\n", old_size_block - node->size - (15 - roundUpDiff(((char *)node + STRUCT_SIZE + size_of_block+ 1), 16)) - 1);

	new_node->buffer_overflow = MAGIC_NUMBER;
	new_node->is_busy = ALLOC_FREE;
	new_node->size = old_size_block - node->size - (15 - roundUpDiff(((char *)node + STRUCT_SIZE + size_of_block+ 1), 16)) - 1;
	//new_node->size = old_size_block - node->size;
	new_node->next = node->next;
	new_node->prev = node;
	new_node->block = node->block;

	/*if (((char *)new_node + new_node->size) > ((char *)new_node->block + new_node->block->total_size)) {
		////printk("Split : Pointer way to big !\n");
		new_node->size = ((char *)new_node->block + new_node->block->total_size) - ((char*)new_node) - 1;
		////printk("Split : resized pointer to %lu !\n", ((char *)new_node->block + new_node->block->total_size) - ((char*)new_node) - 1);
	}*/

	////printk("New_node->size %lu - %lu = %lu\n", old_size_block, node->size, old_size_block - node->size);
	////printk("Node->size (%p) is %lu, and new_node->size (%p) = %lu\n", node, node->size, new_node, new_node->size);

	if (node->next)
		node->next->prev = new_node;
	node->next = new_node;
	//////printk("new_node->block = %p\n", new_node->block);
	//////printk("Node->next = %p\n", node->next);
	//////printk("node->buffer_overflow = %d\n", node->buffer_overflow);
	//////printk("New_node->buffer_overflow = %d\n", new_node->buffer_overflow);
	//////printk("Placed g_curr_node @ %p\n", g_curr_node);
	////printk("Node %p -> %p, new_node %p -> %p\n", node, ((char *)node + node->size), new_node, ((char *) new_node + new_node->size));
	return new_node;
}

t_alloc		*find_place_at_beginning(size_t size_looked) {
	t_alloc *node_tmp;

	node_tmp = g_curr_node;
	////printk("find_place_at_beginning block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		//////printk("Reversing linked list : Actually on %p, going on %p\n", node_tmp, node_tmp->prev);
		if (node_tmp->prev->buffer_overflow != MAGIC_NUMBER) {
			//////printk("Probably node corruption on %p\n", node_tmp->prev);
			//return 0;
		}
		if (node_tmp->is_busy == ALLOC_FREE && node_tmp->size > (int)size_looked) {
			//printk("(find_place_at_beginning) Found space for %lu at %p (%lu bytes available in the node)\n", size_looked, node_tmp, node_tmp->size);
			//if (node_tmp->block->total_freed_node == node_tmp->block->total_node)
			//	node_tmp->block->total_freed_node--;
			return node_tmp;
		}
		node_tmp = node_tmp->prev;
	}
	////printk("find_place_at_beginning No node able to contain it found\n");
	return 0;
}

t_alloc *should_split(size_t tmp_value, size_t size) {
	t_alloc *return_node_ptr;

	////printk("roundUpDiff return %lu for pointer %p\n", (int)roundUpDiff(((char *)g_curr_node + tmp_value + 1), 16), ((char *)g_curr_node + tmp_value + 1));
	////printk("Split ? g_curr_node at addr (%p) size %lu - %lu - %lu - %lu - 1 = %d > 0 = %s\n",
	/*g_curr_node,
	g_curr_node->size,
	tmp_value,
	roundUpDiff(((char *)g_curr_node + tmp_value + 1), 16),
	STRUCT_SIZE,
	(int)g_curr_node->size - (int)tmp_value - (int)roundUpDiff(((char *)g_curr_node + tmp_value + 1), 16) - (int)STRUCT_SIZE - 1,
	((int)g_curr_node->size - (int)tmp_value - (int)roundUpDiff(((char *)g_curr_node + tmp_value + 1), 16) - (int)STRUCT_SIZE - 1 > 0) ? "YES" : "NO");*/

	// Split node if needed
	if ((int)g_curr_node->size - (int)tmp_value - (int)roundUpDiff(((char *)g_curr_node + tmp_value + 1), 16) - (int)STRUCT_SIZE - 1 > 0) {
		return_node_ptr = g_curr_node;
		g_curr_node = split_node(g_curr_node, size);
	} else {
		////printk("Node %p is marqued as not available\n", g_curr_node);
		////printk("Node %p has size %lu\n", g_curr_node, g_curr_node->size);
		g_curr_node->is_busy = ALLOC_USED;

		return_node_ptr = g_curr_node;
	}
	return return_node_ptr;
}

void	*real_malloc(size_t size) {
	t_alloc *return_node_ptr = 0;
	t_alloc *tmp_g_curr_node = 0;
	t_alloc *tmp2_g_curr_node = 0;

	//////printk("int %d, short %d, long %d, size_t %d\n", sizeof(int), sizeof(short), sizeof(long), sizeof(size_t));

	(void)tmp_g_curr_node;
	printk("-------REQUESTING NEW MALLOC OF SIZE %lu---------\n", size);
	if (size == 0)
		return NULL;
	if (!g_curr_node) {
		if (!create_link_new_node(size))
			return NULL;
		////printk("Head of linked list is now init @ %p\n", g_curr_node);
	}
	printk("Start of alloc -> g_curr_node = %p next-> %p, prev %p with size %lu\n", g_curr_node, g_curr_node->next, g_curr_node->prev,  g_curr_node->size);
	int tmp_value = STRUCT_SIZE + size;
	if (g_curr_node->is_busy == ALLOC_USED || tmp_value > g_curr_node->size) {

		//if (g_curr_node->is_busy == ALLOC_USED)
			////printk("Creating new node cause g_curr_node is set as locked\n");
		//else
			////printk("Creating new node cause size (%lu) > g_curr_node->size (%lu)\n", tmp_value, g_curr_node->size);

		if (!(tmp_g_curr_node = find_place_at_beginning(tmp_value))) {
			if (!create_link_new_node(size))
				return NULL;
		} else {
			tmp2_g_curr_node = g_curr_node;
			g_curr_node = tmp_g_curr_node;
		}
		return_node_ptr = should_split(tmp_value, size);
	} else if (g_curr_node->buffer_overflow == MAGIC_NUMBER) {
		////printk("Found space for %lu (%lu + %lu) bytes in block located at %p (%lu bytes available)\n", tmp_value, STRUCT_SIZE, size, g_curr_node, g_curr_node->size);
		return_node_ptr = should_split(tmp_value, size);
	} else {
		////printk("tmp_value = %d, g_curr_node->Size = %d\n", tmp_value, g_curr_node->size);
		////printk("Possibly corrupted node on %p, magic number is %d, should be %d\n", g_curr_node, g_curr_node->buffer_overflow, MAGIC_NUMBER);
		return NULL;
	}

	if (tmp2_g_curr_node) {
		g_curr_node = tmp2_g_curr_node;
		tmp2_g_curr_node = 0;
	}
	////printk("before return, g_curr_node is %p\n", g_curr_node);

	printk("g_curr_node = %p and g_curr_node->next %p before return\n", g_curr_node, g_curr_node->next);
	////printk("Incrementing total node for pointer %p, bringing it to %d\n", return_node_ptr->block, return_node_ptr->block->total_node);
	////printk("Total node for pointer %p is %d\n", return_node_ptr->block, return_node_ptr->block->total_node);
	//////printk("Size of t_alloc is %lu and size of t_bloc is %lu\n", sizeof(t_alloc), sizeof(t_bloc));
	//////printk("Final check before launching, is pointer aligned ? %s\n",
	//		(((uintptr_t)((char *)return_node_ptr + STRUCT_SIZE + 1) % 16) == 0) ? "YES" : "NO");
	////printk("Block begin at %p and end at %p size of %lu\n", return_node_ptr->block, ((char*)return_node_ptr->block + return_node_ptr->block->total_size), return_node_ptr->block->total_size);
	////printk("Node check before return : prev -> %p and next -> %p\n", return_node_ptr->prev, return_node_ptr->next);
	////printk("Alloc metadata at %p, datas at %p and end at %p.\n", return_node_ptr, ((char *)return_node_ptr + STRUCT_SIZE + 1), ((char *)return_node_ptr + return_node_ptr->size));
	////printk("Returning %p with size %lu (real size %lu) from malloc call. Original ptr is %p\n", ((char *)return_node_ptr + STRUCT_SIZE + 1), return_node_ptr->size - STRUCT_SIZE, return_node_ptr->size, return_node_ptr);
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
