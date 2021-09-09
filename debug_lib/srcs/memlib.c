/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memlib.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/17 00:24:32 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/09 17:26:29 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memlib.h"

void	*ft_memset(void *s, int c, size_t n)
{
	char	*t;
	int		i;

	t = (char *)s;
	i = 0;
	while (i != (int)n)
		t[i++] = c;
	return (t);
}

void	*ft_memcpy(void *s1, void const *s2, size_t n)
{
	int		i;
	char	*t1;
	char	*t2;

	i = 0;
	t1 = (char *)s1;
	t2 = (char *)s2;
	while (i != (int)n)
	{
		t1[i] = t2[i];
		i++;
	}
	return (t1);
}
