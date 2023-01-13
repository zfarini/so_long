/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:25:34 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/19 10:14:56 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t n) // todo: change the implementation
{
	size_t	i;
	int		d;

	if (src > dst)
	{
		i = 0;
		d = 1;
	}
	else
	{
		i = n - 1;
		d = -1;
	}
	while (i < n)
	{
		((char *)dst)[i] = ((char *)src)[i];
		if (!i && d == -1)
			break ;
		i += d;
	}
	return (dst);
}
