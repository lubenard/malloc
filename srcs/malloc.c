/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 13:50:12 by lubenard          #+#    #+#             */
/*   Updated: 2021/08/26 16:10:53 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/mman.h>

#include <malloc.h>

t_alloc *g_head = 0;

t_alloc *init_node() {
	t_alloc *node;
	node = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 4096);
	node->size = 4096;
	node->size_remaining = 4096;
	node->is_free = 0;
	return node;
}

void	*malloc(size_t size) {
	(void)size;
	if (g_head == 0) {
		printf("Should init head\n");
		g_head = init_node();
		printf("Head is now init @ %p\n", g_head);
	}
	return 0;
}

void	free(void *ptr) {
	if (ptr == 0)
		return;
}

void	*realloc(void *ptr, size_t size) {
	(void)ptr;
	(void)size;
	return 0;
}
