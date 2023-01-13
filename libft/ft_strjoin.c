/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:48:48 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/19 10:16:52 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*s;
	int		l1;
	int		l2;

	l1 = ft_strlen(s1);
	l2 = ft_strlen(s2);
	s = malloc(l1 + l2 + 1);
	if (!s)
		return (0);
	ft_memcpy(s, s1, l1);
	ft_memcpy(s + l1, s2, l2);
	s[l1 + l2] = 0;
	return (s);
}

