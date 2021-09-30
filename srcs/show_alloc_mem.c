/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:37:19 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/30 18:18:45 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

// Debug
#include "../debug_lib/srcs/iolib.h"
#include "../debug_lib/srcs/memlib.h"

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

int		isprint(int c) {
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}

void int_into_str_hex(unsigned int addr, char *result, int size) {
	int		len;
	char	base_str[16] = "0123456789abcdef";

	len = size - 1;
	ft_memset(result, '0', size);
	result[size - 1] = 0;
	while (addr != 0) {
		result[--len] = base_str[addr % 16];
		addr = addr / 16;
	}
}

void print_stack(size_t beginning, size_t ending) {
	int i;
	char *addr;
	char hex[3];
	while (beginning <= ending) {
		i = 0;
		printk("0x%x ", beginning);
		addr = (char*)beginning;
		while (i != 10) {
			int_into_str_hex(addr[i++], hex, 3);
			printk("%s ", hex);
		}
		i = 0;
		while (i != 10) {
			if (isprint(addr[i]))
				printk("%c", addr[i]);
			else
				printk(".");
			i++;
		}
		beginning += 10;
		printk("\n");
	}
}

void print_type(t_alloc *node, int type) {
	if (type == TINY)
		printk("TINY : %x\n", node);
	else if (type == SMALL)
		printk("SMALL : %x\n", node);
	else
		printk("LARGE : %x\n", node);
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
			printk("%x - %x : %lu octets\n", ((char *)node + sizeof(t_alloc) + 1), ((char *) node->next - 1), node->size - sizeof(t_alloc));
			total_size += node->size - sizeof(t_alloc);
		}
		node = node->next;
	}
	printk("Total : %lu octets\n", total_size);
}

void show_alloc_mem_ex() {
	t_alloc *node;
	size_t total_size = 0;

	node = g_curr_node;
	while (node->prev) {
		node = node->prev;
	}

	printk("----Start showing node list----\n");
	while (node) {
		printk("---Node---\n");
		printk("Node address %p\n", node);
		printk("Pointer returned is %p\n", ((char*) node + sizeof(t_alloc) + 1));
		printk("Node size is %lu\n", node->size);
		printk("Node is %s\n", (node->is_busy == 1) ? "Available" : "Occupied");
		printk("Node next is %p\n", node->next);
		printk("Node prev is %p\n", node->prev);
		if (node->is_busy == 2) {
			printk("Hex content for node %p (size %lu): \n", node, node->size - sizeof(t_alloc));
			print_stack((size_t)((char *)node + sizeof(t_alloc) + 1),(size_t)((char *)node + node->size));
			total_size += node->size - sizeof(t_alloc);
		}
		node = node->next;
	}
	printk("----End showing node list----\n");
	printk("Total : %lu octets\n", total_size);
}
