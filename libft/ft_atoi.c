/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:43:17 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/19 10:09:31 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_isspace(int c)
{
	return (c == '\t' || c == '\n' || c == '\f'
		|| c == '\r' || c == '\v' || c == ' ');
}

//TOOD: make atoi work exactly like libc implementation
int	ft_atoi(const char *str)
{
	int			sign;
	long long	res;

	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	res = 0;
	while (ft_isdigit(*str)) 
	{
		if (sign == 1)
		{
			//if (res * 10 > LLONG_MAX)
			res = res * 10 + (*str - '0');
		}
		else
			res = res * 10 - (*str - '0');
		str++;
	}
	return (res);
}
