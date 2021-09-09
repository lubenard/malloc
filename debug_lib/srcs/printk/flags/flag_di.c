/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag_di.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/27 18:28:21 by lubenard          #+#    #+#             */
/*   Updated: 2021/05/03 15:15:30 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/printk.h"

void	flag_di(const char *str, int *i, va_list *ap, t_output *output)
{
	(void)str;
	fill_buffer_nbr(output, va_arg(*ap, int));
	(*i)++;
}
