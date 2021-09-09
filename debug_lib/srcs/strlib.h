/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strlib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/17 00:17:58 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/09 17:27:15 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRLIB_H
# define STRLIB_H

# include <stddef.h>
size_t	ft_strlen(const char *str);
int		ft_isalpha(int c);
int		ft_strchri(const char *s, int c);
void	ft_bzero(void *s, size_t n);
int		ft_isprint(int c);
char	ft_strupper(char *str);
int		ft_strcmp(char const *s1, char const *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_atoi(char const *str);
char	ft_strcpy(char *dest, char const *src);
char	ft_strlcpy(char *dest, char *src, int size);
#endif
