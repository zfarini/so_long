/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 17:48:26 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 17:53:00 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	map_error(t_map *map, char *s)
{
	int	i;

	write(STDERR_FILENO, "Error\n", 6);
	write(STDERR_FILENO, s, ft_strlen(s));
	write(STDERR_FILENO, "\n", 1);
	if (map->arr)
	{
		i = 0;
		while (i < map->height)
		{
			free(map->arr[i]);
			i++;
		}
		free(map->arr);
	}
	close(map->fd);
	exit(1);
}

char	*append_char(t_map *map, char *s, char c)
{
	char	*res;
	size_t	len;

	len = ft_strlen(s);
	res = malloc(len + 2);
	if (!res)
		map_error(map, "malloc failed");
	ft_memcpy(res, s, len);
	res[len] = c;
	res[len + 1] = 0;
	free(s);
	return (res);
}

char	*get_next_line(t_map *map, int fd)
{
	char	*s;
	char	c;
	int		ret;

	s = 0;
	while (1)
	{
		ret = read(fd, &c, 1);
		if (ret < 0)
		{
			free(s);
			map_error(map, "failed to read map");
		}
		else if (!ret || c == '\n')
			break ;
		s = append_char(map, s, c);
	}
	return (s);
}

void	*ft_realloc(t_map *map, void *ptr, size_t old_size, size_t new_size)
{
	void	*new;

	new = malloc(new_size);
	if (!new)
	{
		map_error(map, "malloc failed");
		return (0);
	}
	ft_memcpy(new, ptr, old_size);
	free(ptr);
	return (new);
}

int	check_if_string_have_one_unique_char(char *s, char c)
{
	while (*s)
	{
		if (*s != c)
			return (0);
		s++;
	}
	return (1);
}
