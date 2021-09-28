/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:37:19 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/28 11:49:39 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"


// Debug
#include "../debug_lib/srcs/iolib.h"

extern t_alloc *g_curr_node;

void show_alloc_mem() {
	t_alloc *node;

	node = g_curr_node;
	while (node->prev) {
		node = node->prev;
	}

	printk("----Start showing linked list----\n");
	while (node) {
		printk("---Node---%s\n", (g_curr_node == node) ? " <-- This is g_curr_node" : "");
		printk("Node address %p\n", node);
		printk("Pointer returned is %p\n", ((char*) node + sizeof(t_alloc) + 1));
		printk("size is %lu\n", node->size);
		printk("is_free %d\n", node->is_busy);
		printk("next %p\n", node->next);
		printk("prev %p\n", node->prev);
		node = node->next;
	}
	printk("----End of linked list----\n");

}
