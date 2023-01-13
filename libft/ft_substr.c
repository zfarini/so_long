/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:47:18 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/13 18:27:06 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	slen;
	size_t	i;
	char	*res;

	slen = ft_strlen(s);
	if (start >= slen)
		return (ft_calloc(1, 1));
	if (start + len <= slen)
		res = malloc(len + 1);
	else
		res = malloc(slen - start + 1);
	if (!res)
		return (0);
	i = 0;
	while (start < slen && i < len)
		res[i++] = s[start++];
	res[i] = '\0';
	return (res);
}
