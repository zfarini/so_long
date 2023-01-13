/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:42:29 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/19 10:19:07 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;

	if (!needle[0])
		return ((char *)haystack);
	i = 0;
	while (i < len && haystack[i]) // todo: make sure this function is ok
	{
		j = 0;
		while (i + j < len && needle[j] && needle[j] == haystack[i + j])
			j++;
		if (!needle[j])
			return ((char *)haystack + i);
		i++;
	}
	return (0);
}
