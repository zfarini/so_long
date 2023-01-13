/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:50:00 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/13 18:28:37 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s, char const *set)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (s[i] && ft_strchr(set, s[i]))
		i++;
	if (!s[i])
		return (ft_calloc(1, 1));
	j = ft_strlen(s) - 1;
	while (j > i && ft_strchr(set, s[j]))
		j--;
	return (ft_substr(s, i, j - i + 1));
}
