/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/13 23:35:18 by lubenard         ###   ########.fr       */
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
#include "../debug_lib/srcs/strlib.h"

t_block *curr_block_start;
t_block *curr_block_end;

// END DEBUG ONLY
size_t roundUp(void *a, size_t b) {
	return (1 + ((size_t)a - 1) / b) * b;
}

size_t roundUpDiff(void *a, size_t b) {
	return (size_t)roundUp(a, b) - (size_t)a;
}

t_alloc *init_node(size_t size_requested) {
	t_block *bloc;
	t_alloc *node;

	size_requested += TOTAL_STRUCT_SIZE + 1;
	size_requested = (size_requested / PAGESIZE + 1) * PAGESIZE;
	bloc = mmap(NULL, size_requested, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	node = (t_alloc *)((char *)bloc + STRUCT_BLOCK_SIZE + 1);
	if (!bloc || bloc == MAP_FAILED)
		return 0;
	bloc->total_node = 0;
	bloc->total_freed_node = 0;
	bloc->total_size = size_requested - 1;
	curr_block_start = bloc;
	curr_block_end = (t_block *)((char *)bloc + bloc->total_size);
	node->size = bloc->total_size - STRUCT_SIZE - 1;
	node->buffer_overflow = MAGIC_NUMBER;
	// Available: 1, Not Available: 2
	node->is_busy = ALLOC_FREE;
	node->next = NULL;
	node->block = bloc;
	node->prev = NULL;
	bloc->next = NULL;
	bloc->prev = (g_curr_node) ? g_curr_node->block : NULL;
	if (g_curr_node)
		g_curr_node->block->next = bloc;
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
	return 1;
}

t_alloc *split_node(t_alloc *node, size_t size_of_block) {
	t_alloc *new_node;

	size_t old_size_block = node->size;

	new_node = (t_alloc *)roundUp(((char *)node + STRUCT_SIZE + size_of_block + 1), 16);

	node->size = (size_t)new_node - (size_t)node - 1;
	node->block->total_node++;
	node->is_busy = ALLOC_USED;

	new_node->buffer_overflow = MAGIC_NUMBER;
	new_node->is_busy = ALLOC_FREE;
	new_node->size = old_size_block - node->size - (15 - roundUpDiff(((char *)node + STRUCT_SIZE + size_of_block+ 1), 16)) - 1;
	new_node->next = node->next;
	new_node->prev = node;
	new_node->block = node->block;

	if (node->next)
		node->next->prev = new_node;
	node->next = new_node;
	return new_node;
}

t_alloc		*find_place_at_beginning(size_t size_looked) {
	t_alloc *node_tmp;

	node_tmp = g_curr_node;
	while (node_tmp->prev) {
		if (node_tmp->prev->buffer_overflow != MAGIC_NUMBER)
			return 0;
		if (node_tmp->is_busy == ALLOC_FREE && node_tmp->size >= (int)size_looked)
			return node_tmp;
		node_tmp = node_tmp->prev;
	}
	return 0;
}

t_alloc *should_split(size_t tmp_value, size_t size) {
	t_alloc *return_node_ptr;

	// Split node if needed
	if ((int)g_curr_node->size - (int)tmp_value - (int)roundUpDiff(((char *)g_curr_node + tmp_value + 1), 16) - (int)STRUCT_SIZE - 1 > 0) {
		return_node_ptr = g_curr_node;
		g_curr_node = split_node(g_curr_node, size);
	} else {
		g_curr_node->is_busy = ALLOC_USED;
		return_node_ptr = g_curr_node;
	}
	return return_node_ptr;
}

void	*real_malloc(size_t size) {
	t_alloc *return_node_ptr = 0;
	t_alloc *tmp_g_curr_node = 0;
	t_alloc *tmp2_g_curr_node = 0;

	if (size == 0)
		return NULL;
	if (!g_curr_node) {
		if (!create_link_new_node(size))
			return NULL;
	}
	int tmp_value = STRUCT_SIZE + size;
	if (g_curr_node->is_busy == ALLOC_USED || tmp_value > g_curr_node->size) {
		if (!(tmp_g_curr_node = find_place_at_beginning(tmp_value))) {
			if (!create_link_new_node(size))
				return NULL;
		} else {
			tmp2_g_curr_node = g_curr_node;
			g_curr_node = tmp_g_curr_node;
		}
		return_node_ptr = should_split(tmp_value, size);
	} else if (g_curr_node->buffer_overflow == MAGIC_NUMBER)
		return_node_ptr = should_split(tmp_value, size);
	else
		return NULL;

	if (tmp2_g_curr_node) {
		g_curr_node = tmp2_g_curr_node;
		tmp2_g_curr_node = 0;
	}
	return ((char *)return_node_ptr + STRUCT_SIZE + 1);
}

void	*malloc(size_t size) {
	void *return_ptr;

	pthread_mutex_lock(&g_mutex);
	return_ptr = real_malloc(size);
	pthread_mutex_unlock(&g_mutex);
	return return_ptr;
}
