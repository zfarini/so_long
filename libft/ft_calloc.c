/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:44:15 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/19 10:13:52 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*res;

	if (size && count > SIZE_MAX / size) // Make sure this is right in all cases
		return (0);
	res = malloc(count * size);
	if (res)
		ft_bzero(res, count * size);
	return (res);
}
