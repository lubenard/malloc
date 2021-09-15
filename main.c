/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 14:36:09 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/15 14:12:42 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/malloc.h"
#include "debug_lib/srcs/iolib.h"
#include <stdio.h>

char	*ft_strncpy(char *dest, char const *src, unsigned int n) {
	unsigned int i;

	i = 0;
	while (src[i] && i < n) {
		dest[i] = src[i];
		i++;
	}
	while (i < n) {
		dest[i++] = '\0';
	}
	return (dest);
}

int main(void) {
	char *str;
	int i = 0;
	while (i != 100) {
		str = malloc(20);
		printk("I = %d\n", i);
		i++;
	}
	//ft_strncpy(str, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 6);
	//printf("[MAIN TEST] String is '%s'\n", str);
	//char *str2 = malloc(0);
	//char *str = malloc(5000);
	//char *str3 = malloc(10);
	/*char *str4 = malloc(10);
	char *str5 = malloc(10);*/
	/*free(str4);
	free(str5);*/
}
