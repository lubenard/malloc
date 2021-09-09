/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lubenard <lubenard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 19:26:18 by lubenard          #+#    #+#             */
/*   Updated: 2021/09/09 17:28:53 by lubenard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include "include/printk.h"

/*
 * We are forced to pass va_list with address or it will only print one arg.
 * Normally, the va_arg update the index to fetch the next argument.
 * However, it seems there is a diference of type between i386 and amd64 for 
 * va_list format.
 * If in amd64, just passing the struct is okay and update the index, we need 
 * to pass the address in i386
 * See here for more infos:
 * https://stackoverflow.com/a/52356204/9108386
 */

void printk(const char *str, ...) {
	va_list ap;

	if (str != NULL) {
		va_start(ap, str);
		parsing(str, &ap);
		va_end(ap);
	}
}
