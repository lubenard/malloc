/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/24 12:18:06 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/mman.h>

#include "malloc.h"

//TODO: Used for ft_memcpy, clean it later
#include "../debug_lib/srcs/memlib.h"
t_alloc *g_curr_node = 0;

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
	t_alloc *node;

	printk("Creating NEW node: %lu bytes long (%lu + %lu) <- sizeof(t_alloc)\n", size_requested + sizeof(t_alloc), size_requested, sizeof(t_alloc));
	size_requested += sizeof(t_alloc);
	size_requested = (size_requested / PAGESIZE + 1) * PAGESIZE;
	printk("Creating NEW node: %lu bytes long\n", size_requested);
	node = mmap(NULL, size_requested, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (!node || node == MAP_FAILED) {
		printk("Map failed\n");
		return 0;
	}
	printk("Node begin at %p and end at %p\n", node, (t_alloc *)((char *)node + size_requested));
	curr_block_start = node;
	curr_block_end = (t_alloc *)((char *)node + size_requested);
	printk("Node pointer is %p (size - sizeof(t_alloc))\n", node);
	node->size = size_requested - sizeof(t_alloc);
	printk("Node->size is %lu\n", node->size);
	node->buffer_overflow = MAGIC_NUMBER;
	// Available: 1, Not Available: 2
	node->is_busy = 1;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

void print_linked_list() {
	t_alloc *node;

	node = g_curr_node;
	while (node->prev) {
		node = node->prev;
	}

	printk("----Start showing linked list----\n");
	while (node) {
		printk("---Node---%s\n", (g_curr_node == node) ? " <-- This is g_curr_node" : "");
		printk("Node address %p\n", node);
		printk("size is %lu\n", node->size);
		printk("is_free %d\n", node->is_busy);
		printk("next %p\n", node->next);
		printk("prev %p\n", node->prev);
		node = node->next;
	}
	printk("----End of linked list----\n");
}

void create_link_new_node(size_t size_of_block) {
	t_alloc *node;

	node = init_node(size_of_block);
	// Link only if g_curr_node exist
	if (g_curr_node) {
		node->prev = g_curr_node;
		g_curr_node->next = node;
	}
	g_curr_node = node;
	printk("Placed g_curr_node @ %p\n", g_curr_node);
}

void split_node(t_alloc *node, size_t size_of_block) {
	t_alloc *new_node;

	size_t old_size_block = node->size;

	//new_node = (t_alloc *)((char*) node + sizeof(t_alloc) + size_of_block + 1);
	printk("Round up return %lu, and converted into addr it is %p, input is %p, test addr is %p\n", roundUp(((char *) node + sizeof(t_alloc) + size_of_block + 1), 16), roundUp(((char *)node + sizeof(t_alloc) + size_of_block + 1), 16), ((char*) node + sizeof(t_alloc) + size_of_block + 1),node + sizeof(t_alloc) + size_of_block + 1);

	new_node = (t_alloc *)((char*) node + sizeof(t_alloc) + size_of_block + 1 + roundUpDiff(((char *)(node + sizeof(t_alloc) + size_of_block + 1)), 16));

	printk("Splitting at addr %p, computed this way : %p + %lu + %zu + %zu + 1 = %p\n", new_node, node, sizeof(t_alloc), size_of_block, roundUpDiff(((char *)node + sizeof(t_alloc) + 1), 16), ((char*) node + sizeof(t_alloc) + size_of_block + 1 + roundUpDiff(((char *)(node + sizeof(t_alloc) + 1)), 16)));

	//printk("roundUpDiff return %lu for input %p\n", roundUpDiff(((char *)node + sizeof(t_alloc) + 1), 16), ((char*) node + sizeof(t_alloc) + size_of_block + 1));
	printk("roundUpDiff return %lu for input %p\n", roundUpDiff(((char *)node + sizeof(t_alloc) + 1), 16), new_node);

	node->size = sizeof(t_alloc) + size_of_block + roundUpDiff(new_node, 16);
	new_node->size = old_size_block - node->size;
	printk("Node->size is %lu, and new_node->size = %lu\n", node->size, new_node->size);
	printk("Node %p is marqued as not available\n", node);
	node->is_busy = 2;
	new_node->is_busy = 1;
	node->buffer_overflow = MAGIC_NUMBER;
	//new_node->next = NULL;
	new_node->next = (node->next) ? node->next : NULL;
	node->next = new_node;
	new_node->prev = node;
	g_curr_node = new_node;
	printk("Placed g_curr_node @ %p\n", g_curr_node);
}

t_alloc		*find_place_at_beginning(size_t size_looked) {
	t_alloc *node_tmp;

	node_tmp = g_curr_node;
	printk("find_place_at_beginning block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		printk("Reversing linked list : Actually on %p, going on %p\n", node_tmp, node_tmp->prev);
		node_tmp = node_tmp->prev;
	}
	printk("Current pointer is %p\n", node_tmp);
	while (node_tmp->next) {
		//printk("Checking for %p, going on %p next\n", node_tmp, node_tmp->next);
		if (node_tmp->is_busy == 1 && node_tmp->size > size_looked) {
			printk("(find_place_at_beginning) Found space for %lu at %p (%lu bytes available in the node)\n", size_looked, node_tmp, node_tmp->size);
			return node_tmp;
		}
		node_tmp = node_tmp->next;
	}
	printk("find_place_at_beginning No node able to contain it found\n");
	return 0;
}

void	*malloc(size_t size) {
	t_alloc *return_node_ptr = 0;
	t_alloc *tmp_g_curr_node = 0;
	t_alloc *tmp2_g_curr_node = 0;

	printk("-------REQUESTING NEW MALLOC OF SIZE %lu---------\n", size);
	if (size == 0)
		return NULL;
	if (!g_curr_node) {
		create_link_new_node(size);
		printk("Head of linked list is now init @ %p\n", g_curr_node);
	}
	if (g_curr_node->is_busy == 2) {
		printk("Creating new node cause g_curr_node is set as locked\n");
		if (!(tmp_g_curr_node = find_place_at_beginning(size + sizeof(t_alloc))))
			create_link_new_node(size);
		else {
			tmp2_g_curr_node = g_curr_node;
			g_curr_node = tmp_g_curr_node;
		}
		printk("Found space for %lu (%lu + %lu) bytes in block located at %p (%lu bytes available)\n", size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16), size, sizeof(t_alloc), g_curr_node, g_curr_node->size);

		printk("Should split ? g_curr_node size %lu - %lu = %d > 0 (size_requested in malloc) = %s\n", g_curr_node->size, size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16), (int)g_curr_node->size - ((int)size + (int)sizeof(t_alloc) + (int)roundUpDiff((char *)size + sizeof(t_alloc), 16)), ((int)g_curr_node->size - (int)(size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16)) > 0) ? "YES" : "NO");
		if ((int)g_curr_node->size - (int)(size + sizeof(t_alloc)) > 0) {
			return_node_ptr = g_curr_node;
			split_node(g_curr_node, size);
		} else {
			printk("Node %p is marqued as not available\n", g_curr_node);
			g_curr_node->is_busy = 2;
			return_node_ptr = g_curr_node;
		}
	} else if (size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16) > g_curr_node->size) {
		printk("Size (%lu) > g_curr_node->size (%lu)\n", size + sizeof(t_alloc), g_curr_node->size);
		if (!(tmp_g_curr_node = find_place_at_beginning(size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16))))
			create_link_new_node(size);
		else {
			tmp2_g_curr_node = g_curr_node;
			g_curr_node = tmp_g_curr_node;
		}
		return_node_ptr = g_curr_node;
		//printk("Node %p is marqued as busy (2)\n", g_curr_node);
		//g_curr_node->is_busy = 2;
		//printk("%d - %lu = %d\n", g_curr_node->size, size + sizeof(t_alloc), g_curr_node - (size + sizeof(t_alloc)));
		printk("Should split ? g_curr_node size %lu - %lu = %d > 0 (size_requested in malloc) = %s\n", g_curr_node->size, size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16), (int)g_curr_node->size - ((int)size + (int)sizeof(t_alloc) + (int)roundUpDiff((char *)size + sizeof(t_alloc), 16)), ((int)g_curr_node->size - (int)(size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16)) > 0) ? "YES" : "NO");
		if ((int)g_curr_node->size - (int)(size + sizeof(t_alloc) + roundUpDiff((char *)size + sizeof(t_alloc), 16)) > 0)
			split_node(g_curr_node, size);
	} else {
		printk("Found space for %lu (%lu + %lu) bytes in block located at %p (%lu bytes available)\n", size + sizeof(t_alloc), size, sizeof(t_alloc), g_curr_node, g_curr_node->size);
		//We need to split the block from other blocks
		printk("Should split ? g_curr_node size %lu - %lu = %d > 0 (size_requested in malloc) = %s\n", g_curr_node->size, size + sizeof(t_alloc), (int)g_curr_node->size - ((int)size + (int)sizeof(t_alloc)), ((int)g_curr_node->size - (int)(size + sizeof(t_alloc)) > 0) ? "YES" : "NO");
		if ((int)g_curr_node->size - (int)(size + sizeof(t_alloc)) > 0) {
			return_node_ptr = g_curr_node;
			split_node(g_curr_node, size);
		} else {
			printk("Node %p is marqued as not available\n", g_curr_node);
			g_curr_node->is_busy = 2;
			return_node_ptr = g_curr_node;
		}
	}
	printk("Node begin at %p and end at %p\n", curr_block_start, curr_block_end);
	printk("Returning %p from malloc call. Original ptr is %p\n", ((char *)return_node_ptr + sizeof(t_alloc) + 1), return_node_ptr);
	//print_linked_list();
	if (tmp2_g_curr_node) {
		g_curr_node = tmp2_g_curr_node;
		tmp2_g_curr_node = 0;
	}
	printk("~~~~~~~END MALLOC~~~~~~~~~\n");
	return (void *)roundUp(((char*) return_node_ptr + sizeof(t_alloc) + 1), 16);
}

// TODO: Rework this function
void merge_blocks(t_alloc *node_ptr) {
	t_alloc *node_tmp;
	t_alloc *node_tmp_2;

	node_tmp = node_ptr;
	printk("Merge block, actually on %p\n", node_tmp);
	while (node_tmp->prev) {
		printk("Reversing linked list : Actually on %p, going on %p\n", node_ptr, node_tmp->prev);
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
	printk("---------REQUESTING FREE------------\n");
	printk("Getting %p from arg\n", ptr);
	node_ptr = (t_alloc *)((char*) ptr - sizeof(t_alloc) - 1);
	//node_ptr->is_busy = 0;
	printk("Freeing from address %p\n", node_ptr);
	printk("----------END FREE---------------\n");
	//merge_blocks(node_ptr);
	//munmap(node_ptr, sizeof(t_alloc) + node_ptr->size);
}


void	*realloc(void *ptr, size_t size) {
	void *ptr_realloc;
	printk("---REQUEST REALLOC-----\n");

	ptr_realloc = malloc(size);
	//if (ptr)
	//	ft_memcpy(ptr_realloc, ptr, ((t_alloc *)((char*) ptr - sizeof(t_alloc) - 1))->size - sizeof(t_alloc));
	free(ptr);
	printk("----END REALLOC----\n");
	return ptr_realloc;
}

/*void	*calloc(size_t nitems, size_t size) {
	void	*ptr;

	printk("---REQUEST CALLOC-----\n");
	size *= nitems;
	if (!(ptr = malloc(size)))
		return (NULL);
	ft_bzero(ptr, size);
	printk("----END CALLOC----\n");
	return (ptr);
}*/
