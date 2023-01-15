/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 17:56:08 by zfarini           #+#    #+#             */
/*   Updated: 2022/10/07 13:21:13 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*find_first_occ(char const *s, char c)
{
	char	*res;

	res = ft_strchr(s, c);
	if (!res)
		res = ft_strchr(s, 0);
	return ((char *)res);
}

static int	ft_splitcount(const char *s, char c)
{
	int	res;

	res = 0;
	while (s && *s)
	{
		while (*s == c)
			s++;
		if (*s == '\0')
			break ;
		res++;
		s = find_first_occ(s, c);
	}
	return (res);
}

static void	free_split_res(char **a)
{
	int	i;

	i = 0;
	while (a[i])
		free(a[i++]);
	free(a);
}

static int	add_str(char **res, int i, char const **s, char c)
{
	int	j;

	res[i] = malloc(find_first_occ(*s, c) - *s + 1);
	if (!res[i])
	{
		free_split_res(res);
		return (0);
	}
	j = 0;
	while (**s && **s != c)
	{
		res[i][j++] = **s;
		*s = *s + 1;
	}
	res[i++][j] = '\0';
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**res;
	int		i;

	i = 0;
	res = malloc((ft_splitcount(s, c) + 1) * sizeof(char *));
	if (!res)
		return (0);
	while (*s)
	{
		while (*s == c)
			s++;
		if (!(*s))
			break ;
		if (!add_str(res, i, &s, c))
			return (0);
		i++;
	}
	res[i] = 0;
	return (res);
}
