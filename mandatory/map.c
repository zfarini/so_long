/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 17:42:16 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 18:48:14 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	dfs(t_map *map, int row, int col, int *visited)
{
	if (row < 0 || col < 0 || row >= map->height || col >= map->width
		|| *(visited + row * map->width + col) || map->arr[row][col] == '1')
		return ;
	*(visited + row * map->width + col) = 1;
	if (map->arr[row][col] == 'E')
		return ;
	dfs(map, row - 1, col, visited);
	dfs(map, row + 1, col, visited);
	dfs(map, row, col - 1, visited);
	dfs(map, row, col + 1, visited);
}

void	check_map_for_valid_path(t_map *map)
{
	int	i;
	int	j;
	int	*visited;

	visited = ft_calloc(map->width * map->height, sizeof(int));
	if (!visited)
		map_error(map, "malloc failed");
	dfs(map, map->player_pos / map->width,
		map->player_pos % map->width, visited);
	i = 0;
	while (i < map->height)
	{
		j = 0;
		while (j < map->width)
		{
			if ((map->arr[i][j] == 'C' || map->arr[i][j] == 'E')
				&& !(*(visited + i * map->width + j)))
				map_error(map, "map doesn't have any valid path");
			j++;
		}
		i++;
	}
}

void	check_map_row(t_map *map, int i)
{
	size_t	len;
	int		j;

	len = ft_strlen(map->arr[i]);
	if (len > 0 && map->arr[i][len - 1] == '\n')
		len--;
	if (len != (size_t)map->width)
		map_error(map, "the map must be rectangular");
	if (map->arr[i][0] != '1' || map->arr[i][map->width - 1] != '1'
		|| ((i == 0 || i == map->height - 1)
			&& !check_if_string_have_one_unique_char(map->arr[i], '1')))
		map_error(map, "the map must be closed by walls");
	j = 0;
	while (j < map->width)
	{
		if (!ft_strchr("01CEP", map->arr[i][j]))
			map_error(map, "the map can be composed of only these"
				"5 characters: '0', '1', 'C', 'E', 'P'");
		if (map->arr[i][j] == 'P')
			map->player_pos = i * map->width + j;
		map->p_count += (map->arr[i][j] == 'P');
		map->e_count += (map->arr[i][j] == 'E');
		map->collectibles_count += (map->arr[i][j] == 'C');
		j++;
	}
}

void	check_if_map_is_valid(t_map *map)
{
	int	i;

	if (!map->width || !map->height)
		map_error(map, "the map must be closed by walls");
	i = 0;
	while (i < map->height)
	{
		check_map_row(map, i);
		i++;
	}
	if (map->p_count != 1 || map->e_count != 1 || map->collectibles_count < 1)
		map_error(map, "the map must contain 1 exit, at least 1 collectible"
			", and 1 starting position to be valid");
	check_map_for_valid_path(map);
}

void	parse_map(t_map *map, char *map_filename)
{
	char	*line;
	size_t	len;

	ft_memset(map, 0, sizeof(*map));
	map->fd = open(map_filename, O_RDONLY);
	if (map->fd < 0)
		map_error(map, "failed to read map");
	while (1)
	{
		line = get_next_line(map, map->fd);
		if (!line)
			break ;
		len = ft_strlen(line);
		if (len > 1 && line[len - 1] == '\n')
			len--;
		if (!map->width)
			map->width = len;
		map->arr = ft_realloc(map, map->arr, map->height * sizeof(char *),
				(map->height + 1) * sizeof(char *));
		map->arr[map->height] = line;
		map->height++;
	}
	check_if_map_is_valid(map);
	close(map->fd);
}
