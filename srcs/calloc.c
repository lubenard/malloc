/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 13:38:02 by lubenard          #+#    #+#             */
/*   Updated: 2021/10/13 23:30:13 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"
#include "../debug_lib/srcs/strlib.h"
#include <pthread.h>

extern pthread_mutex_t g_mutex;

void	*calloc(size_t nitems, size_t size) {
	void	*ptr;

	pthread_mutex_lock(&g_mutex);
	size *= nitems;
	if (!(ptr = real_malloc(size + 1)))
		return (NULL);
	ft_bzero(ptr, size + 1);
	pthread_mutex_unlock(&g_mutex);
	return (ptr);
}
