/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 14:36:09 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/22 15:45:44 by lubenard         ###   ########.fr       */
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
	char *str2;
	int i = 1;
	int j = 0;

	while (i != 100000) {
		str = malloc(i + 1);
		printk("I = %d, donc j = %d. Pointer received is %p\n", i, i - 1, str);
		while (j != i) {
			//printk("i = %lu, 'a' at %p\n",j,  &str[j]);
			str[j++] = 'a';
		}
		str[i] = '\0';
		printk("(Malloc) I = %d, placed backslash at %p\n", i, &str[i]);
		j = 0;

		str2 = realloc(str, i * 2);
		while (j != i * 2) {
			//printk("i = %lu, 'a' at %p\n",j,  &str[j]);
			str2[j++] = 'a';
		}
		str2[i] = '\0';
		printk("(Realloc) I = %d, placed backslash at %p\n", i * 2, &str[i]);
		free(str2);

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
