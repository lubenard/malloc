/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_buffer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/27 17:10:18 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/09 17:32:29 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/printk.h"
#include "unistd.h"

void fill_buffer_nbr_u(t_output *output, unsigned int n) {
	int i;

	i = 0;
	if (n >= 4294967295)
	{
		n = 294967295;
		fill_buffer(output, '4', &i);
	}
	if (n > 9)
	{
		fill_buffer_nbr(output, n / 10);
		fill_buffer_nbr(output, n % 10);
	}
	else
		fill_buffer(output, n + '0', &i);

}

void	fill_buffer_nbr(t_output *output, int n)
{
	int i;

	i = 0;
	if (n >= 2147483647)
	{
		n = 147483647;
		fill_buffer(output, '2', &i);
	}
	else if (n < 0)
	{
		fill_buffer(output, '-', &i);
		if (n <= -2147483648)
		{
			n = 147483648;
			fill_buffer(output, '2', &i);
		}
		else
			n *= -1;
	}
	if (n > 9)
	{
		fill_buffer_nbr(output, n / 10);
		fill_buffer_nbr(output, n % 10);
	}
	else
		fill_buffer(output, n + '0', &i);
}

void fill_buffer_with(t_output *output, int i, char filling)
{
	int j;

	while (i != 0)
	{
		fill_buffer(output, filling, &j);
		i--;
	}
}

void	fill_buffer_rev(t_output *output, char arg[17])
{
	int j;
	int i;

	i = ft_strlen(arg) - 1;
	j = 0;
	while (i != -1)
		fill_buffer(output, arg[i--], &j);
}

void fill_buffer_str(t_output *output, char *fill)
{
	int j;

	j = 0;
	while (fill[j])
		fill_buffer(output, fill[j], &j);
}

void fill_buffer(t_output *output, char fill, int *i)
{
	output->buffer[output->j++] = fill;
	(*i)++;
	if (output->j == PRINTF_BUFF_SIZE)
	{
		write(1, output->buffer, ft_strlen(output->buffer));
		ft_bzero(output->buffer, PRINTF_BUFF_SIZE);
		output->size_string += PRINTF_BUFF_SIZE;
		output->j = 0;
	}
}
