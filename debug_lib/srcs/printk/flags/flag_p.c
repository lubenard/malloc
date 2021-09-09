/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag_p.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/27 17:49:30 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/09 17:33:18 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/printk.h"

/*
** How to convert pointer address into %p ?
** Convert it in unsigned long, then convert it in hexa
** Then add 0x before
*/

void	flag_p(const char *str, int *i, va_list *ap, t_output *output)
{
	char	arg[19];
	short	len_arg;

	(void)str;
	ft_bzero(arg, 19);
	convert_into_hexa(va_arg(*ap, unsigned long), arg);
	len_arg = ft_strlen(arg);
	arg[len_arg] = 'x';
	arg[len_arg + 1] = '0';
	fill_buffer_rev(output, arg);
	(*i)++;
}
