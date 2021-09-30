/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:37:19 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/30 16:30:08 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"


// Debug
#include "../debug_lib/srcs/iolib.h"

extern t_alloc *g_curr_node;

int get_node_type(t_alloc *node) {
	size_t real_alloc_size;

	real_alloc_size = node->size - sizeof(t_alloc);
	if (real_alloc_size <= TINY)
		return TINY;
	else if (real_alloc_size <= SMALL)
		return SMALL;
	else
		return LARGE;
}

void print_type(t_alloc *node, int type) {
	if (type == TINY)
		printk("TINY : %p\n", node);
	else if (type == SMALL)
		printk("SMALL : %p\n", node);
	else
		printk("LARGE : %p\n", node);
}

void show_alloc_mem() {
	t_alloc *node;
	size_t total_size = 0;
	// 1024 => TINY, 2048 => SMALL, 4096 => LARGE
	int last_node_type;

	// It's rewind time !
	node = g_curr_node;
	while (node->prev) {
		node = node->prev;
	}

	if (node->is_busy == 2) {
		last_node_type = get_node_type(node);
		print_type(node, last_node_type);
	}
	while (node) {
		if (node->is_busy == 2) {
			if (get_node_type(node) != last_node_type) {
				last_node_type = get_node_type(node);
				print_type(node, last_node_type);
			}
			printk("%p - %p : %lu octets\n", ((char *)node + sizeof(t_alloc) + 1), ((char *) node->next - 1), node->size - sizeof(t_alloc));
		total_size += node->size - sizeof(t_alloc);
		}
		node = node->next;
	}
	printk("Total : %lu octets\n", total_size);
}
