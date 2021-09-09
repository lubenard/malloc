/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strlib.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/17 00:24:51 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/09 17:27:47 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

size_t ft_strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

int		ft_isalpha(int c)
{
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
		return (1);
	return (0);
}

int		ft_strchri(const char *s, int c)
{
	size_t	n;

	n = 0;
	if (!s || !c)
		return (0);
	while (s[n])
	{
		if (s[n] == c)
			return (n);
		n++;
	}
	return (-1);
}

void	ft_bzero(void *s, size_t n)
{
	int		i;
	char	*t;

	i = 0;
	t = (char *)s;
	while (i != (int)n)
		t[i++] = '\0';
}

int		ft_isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}

char	*ft_strupper(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] >= 97 && str[i] <= 122)
			str[i] -= 32;
		i++;
	}
	return (str);
}

int		ft_strcmp(char const *s1, char const *s2)
{
	int i;

	i = 0;
	if (!s1 || !s2)
		return (-1);
	while (s1[i] == s2[i] && s1[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t i;

	i = 0;
	if (!s1 || !s2)
		return (-1);
	if (n == 0)
		return (0);
	while (s1[i] == s2[i] && s1[i] && s2[i] && i < n - 1)
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int		ft_atoi(char const *str)
{
	size_t	output;
	int		i;
	int		mult;

	output = 0;
	i = 0;
	mult = 1;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\v' \
			|| str[i] == '\n' || str[i] == '\r' || str[i] == '\f')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		mult = (str[i] == '-') ? -1 : 1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		output = output * 10 + str[i] - 48;
		i++;
	}
	return (output * mult);
}

char	*ft_strcpy(char *dest, char const *src)
{
	int i;

	i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*ft_strlcpy(char *dest, char *src, int size)
{
	int i;

	i = 0;
	while (src[i] && i != size)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
