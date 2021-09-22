/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memlib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/17 00:16:46 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/22 15:54:24 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMLIB_H
# define MEMLIB_H

# include <stddef.h>
# include <stdint.h>

void		*ft_memset(void *s, int c, size_t n);
void		*ft_memcpy(void *s1, void const *s2, size_t n);

#endif
