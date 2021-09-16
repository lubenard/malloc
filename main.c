/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 14:36:09 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/16 17:29:52 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "includes/malloc.h"
#include "debug_lib/srcs/iolib.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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
	int i = 1;
	int j = 0;
	write(1, "AAAAAAAAAAAAA\n", 15);
	while (i != 1000) {
		str = malloc(i);
		printk("I = %d, donc j = %d. Pointer received is %p\n", i, i - 1, str);
		while (j != i - 1) {
			//printk("Filling pointer character 'a' at %p\n", &str[j]);
			str[j++] = 'a';
		}
		str[i] = '\0';
		printk("I = %d, str = %s\n", i, str);
		free(str);
		i++;
		j = 0;
	}
	//write(1, "Hello world\n", 15);
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
