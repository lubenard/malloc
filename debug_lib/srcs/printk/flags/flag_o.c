/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag_o.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/27 17:57:08 by lubenard          #+#    #+#             */
/*   Updated: 2021/05/16 23:06:03 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/printk.h"

unsigned long long	convert_into_octal(unsigned int value)
{
	unsigned int	i;
	unsigned int	octal_num;

	octal_num = 0;
	i = 1;
	while (value != 0)
	{
		octal_num += (value % 8) * i;
		value /= 8;
		i *= 10;
	}
	return (octal_num);
}

void	flag_o(const char *str, int *i, va_list *ap, t_output *output)
{
	unsigned long long arg;

	(void)str;
	arg = convert_into_octal(va_arg(*ap, unsigned int));
	fill_buffer_nbr_u(output, arg);
	(*i)++;
}
