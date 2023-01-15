/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 18:19:18 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/07 12:44:23 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_numlen(int n)
{
	int	len;

	len = (n <= 0);
	while (n)
	{
		len++;
		n /= 10;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	int		i;
	char	*res;
	int		sign;

	i = ft_numlen(n);
	sign = 1;
	res = malloc(i + 1);
	if (!res)
		return (0);
	res[i--] = '\0';
	if (n < 0)
	{
		sign = -1;
		res[0] = '-';
	}
	while ((sign == -1 && i >= 1) || (sign == 1 && i >= 0))
	{
		res[i--] = sign * (n % 10) + '0';
		n /= 10;
	}
	return (res);
}
