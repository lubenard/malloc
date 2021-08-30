/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/08/30 21:03:02 by lubenard         ###   ########.fr       */
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
	node->is_free = 1;
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

void place_footer() {
	int *test = (int *)((char*) g_curr_node + sizeof(t_alloc) + g_curr_node->size_remaining);
	printf("Placing magic at address %p\n", test);
	*test = MAGIC_NUMBER;
}

void split_node(t_alloc *node, size_t size_of_block) {
	t_alloc *new_node;

	new_node = (t_alloc *)((char*) node + sizeof(t_alloc) + size_of_block + sizeof(int) + 1);
	printf("Splitting at addr %p, computed this way : %p + %lu + %zu + %lu + 1 = %p\n", new_node, node, sizeof(t_alloc), size_of_block, sizeof(int), ((char*) node + sizeof(t_alloc) + size_of_block + sizeof(int) + 1));
	new_node->size_remaining = node->size_remaining;
	printf("Size of new node is %lu\n", new_node->size_remaining);
	node->size_remaining = size_of_block;
	node->is_free = 0;
	new_node->next = NULL;
	node->next = new_node;
	printf("Size of prev node is %lu\n", node->size_remaining);
	// Add "Footer" to check if it is not overwriting.
	place_footer();
	g_curr_node = new_node;
	printf("Placed g_curr_node @ %p\n", g_curr_node);
}

void	*malloc(size_t size) {
	if (size == 0)
		return NULL;
	printf("-------REQUESTING NEW MALLOC---------\n");
	if (!g_curr_node) {
		create_link_new_node(PAGESIZE);
		printf("Head of linked list is now init @ %p\n", g_curr_node);
	}
	if (size > g_curr_node->size_remaining) {
		create_link_new_node(size);
	} else {
		printf("Found space for %lu bytes in block located at %p\n", size, g_curr_node);
		g_curr_node->size_remaining -= size;
		printf("Remaining size of g_curr_node is %zu\n", g_curr_node->size_remaining);
		//We need to split the block from other blocks
		split_node(g_curr_node, size);
	}
	return g_curr_node + sizeof(t_alloc);
}

void	free(void *ptr) {
	if (ptr == 0)
		return;
	((t_alloc *)ptr - sizeof(t_alloc))->is_free = 1;
}

void	*realloc(void *ptr, size_t size) {
	(void)ptr;
	(void)size;
	return 0;
}
