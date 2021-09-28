/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 11:22:31 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/28 11:57:13 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/malloc.h"
#include "../debug_lib/srcs/memlib.h"
#include <pthread.h>

extern pthread_mutex_t g_mutex;

// Debug
#include "../debug_lib/srcs/iolib.h"
void	*realloc(void *ptr, size_t size) {
	void *ptr_realloc;

	pthread_mutex_lock(&g_mutex);
	printk("---REQUEST REALLOC-----\n");
	ptr_realloc = malloc(size);
	if (ptr)
		ft_memcpy(ptr_realloc, ((char *)ptr - sizeof(t_alloc) - 1), ((t_alloc *)((char*) ptr - sizeof(t_alloc) - 1))->size - sizeof(t_alloc));
	free(ptr);
	printk("----END REALLOC----\n");
	pthread_mutex_unlock(&g_mutex);
	return ptr_realloc;
}

