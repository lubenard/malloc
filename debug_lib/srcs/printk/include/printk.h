/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/27 16:34:34 by lubenard          #+#    #+#             */
/*   Updated: 2021/05/17 00:03:27 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include "../../iolib.h"
# include "../../strlib.h"

# define PRINTF_BUFF_SIZE	1024

# define FT_PRINTF_SHARP	1
# define FT_PRINTF_PLUS		2
# define FT_PRINTF_MINUS	4
# define FT_PRINTF_SPACE	8
# define FT_PRINTF_PREC		16
# define FT_PRINTF_STAR		32
# define FT_PRINTF_ZERO		64

typedef struct	s_output
{
	char	buffer[PRINTF_BUFF_SIZE];
	int		j;
	int		size_string;
}				t_output;

int				parsing(const char *str, va_list *ap);

void			convert_into_hexa(unsigned long value, char ret[17]);

/*
** fill_buffer functions
*/

void			fill_buffer(t_output *output, char fill, int *i);
void			fill_buffer_str(t_output *output, char *fill);
void			fill_buffer_with(t_output *output, int i, char filling);
void			fill_buffer_rev(t_output *output, char arg[17]);
void			fill_buffer_nbr(t_output *output, int nbr);
void			fill_buffer_nbr_u(t_output *output, unsigned int n);
/*
** Flag gestion
*/
void			flag_c(const char *str, int *i, va_list *ap, t_output *output);
void			flag_s(const char *str, int *i, va_list *ap, t_output *output);
void			flag_di(const char *str, int *i, va_list *ap, t_output *output);
void			flag_x(const char *str, int *i, va_list *ap, t_output *output);
void			flag_maj_x(const char *str, int *i, va_list *ap,
						t_output *output);
void			flag_percent(const char *str, int *i, va_list *ap,
						t_output *output);
void			flag_p(const char *str, int *i, va_list *ap, t_output *output);
void			flag_o(const char *str, int *i, va_list *ap, t_output *output);
void			flag_u(const char *str, int *i, va_list *ap, t_output *output);

/*
** Array of pointer
*/

static void (*g_pointer_array[10])(const char *str, int *i, va_list *ap,
				t_output *output) = {
	flag_c, flag_s, flag_di, flag_di, flag_x, flag_maj_x, flag_percent, flag_p,
	flag_o, flag_u
};

#endif
