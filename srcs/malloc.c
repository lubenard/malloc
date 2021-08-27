/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/08/27 17:14:47 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/mman.h>

#include "malloc.h"

t_alloc *g_curr_node = 0;

t_alloc *init_node(size_t size_requested) {
	t_alloc *node;
	node = mmap(NULL, size_requested, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, size_requested);
	node->size_remaining = size_requested - sizeof(t_alloc);
	node->is_free = 0;
	node->next = NULL;
	printf("Just created a node sir, size is %zu\n", node->size_remaining);
	return node;
}

void create_link_new_node(size_t size_of_block) {
	t_alloc *node;

	if (size_of_block > PAGESIZE)
		node = init_node(size_of_block);
	else
		node = init_node(PAGESIZE);
	// Link only if g_curr_node exist
	if (g_curr_node)
		g_curr_node->next = node;
	g_curr_node = node;
}

void split_node(t_alloc *node, size_t size_of_block) {
	t_alloc *new_node;

	new_node = node + sizeof(t_alloc) + size_of_block;
	printf("Splitting at addr %p, computed this way : %p + %lu + %zu = %p\n", new_node, node, sizeof(t_alloc), size_of_block, node + sizeof(t_alloc) + size_of_block);
}

void	*malloc(size_t size) {
	if (!g_curr_node) {
		create_link_new_node(PAGESIZE);
		printf("Head is now init @ %p\n", g_curr_node);
	}
	if (size > g_curr_node->size_remaining) {
		create_link_new_node(size);
	} else {
		g_curr_node->size_remaining -= size;
		printf("Remaining size of g_curr_node is %zu\n", g_curr_node->size_remaining);
		//We need to split the block from other blocks
		split_node(g_curr_node, size);
	}
	return g_curr_node;
}

void	free(void *ptr) {
	if (ptr == 0)
		return;
	((t_alloc *)ptr)->is_free = 1;
}

void	*realloc(void *ptr, size_t size) {
	(void)ptr;
	(void)size;
	return 0;
}
