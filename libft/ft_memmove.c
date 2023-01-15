/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:25:34 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/06 17:27:20 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t n)
{
	int	i;
	int	d;

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
	while (i >= 0 && (size_t)i < n)
	{
		((char *)dst)[i] = ((char *)src)[i];
		i += d;
	}
	return (dst);
}
