#include "so_long.h"

int check_if_string_have_one_unique_char(char *s, char c)
{
	while (*s)
	{
		if (*s != c)
			return (0);
		s++;
	}
	return (1);
}

void	dfs(t_map *map, int row, int col, int *visited)
{
	
	if (row < 0 || col < 0 || row >= map->height || col >= map->width
		|| *(visited + row * map->width + col) || map->contents[row][col] == '1')
		return ;
	*(visited + row * map->width + col) = 1;
	if (map->contents[row][col] == 'E')
		return ;
	dfs(map, row - 1, col, visited);
	dfs(map, row + 1, col, visited);
	dfs(map, row, col - 1, visited);
	dfs(map, row, col + 1, visited);
}

int check_map_for_valid_path(t_map *map)
{
	//find start
	int	i, p_row, p_col;

	i = 0;
	while (i < map->height)
	{
		char *p = ft_strchr(map->contents[i], 'P');
		if (p)
		{
			p_row = i;
			p_col = p - map->contents[i];
			break ;
		}
		i++;
	}
	int *visited = ft_calloc(map->width * map->height, sizeof(int));
	if (!visited)
	{
		printf("Error\nmalloc failed\n");
		return (0);
	}
	dfs(map, p_row, p_col, visited);
	i = 0;
	while (i < map->height)
	{
		for (int j = 0; j < map->width; j++)
		{
			if (map->contents[i][j] != '1' && !(*(visited + i * map->width + j)))
			{
				printf("Error\nMap doesn't have any valid path\n");
				return (0);
			}
		}
		i++;
	}
	return (1);
}

int	check_if_map_is_valid(t_map *map)
{
	int	i;
	int	start;
	int	collectible;
	int	end;

	if (!map->width || !map->height)
	{
		printf("Error\nThe map must contain 1 exit, at least 1 collectible, and 1 starting position to be valid\n");
		return (0);
	}
	i = start = collectible = end = 0;//fuck norm
	while (i < map->height)
	{
		size_t len = ft_strlen(map->contents[i]);
		if (len > 0 && map->contents[i][len - 1] == '\n')
			len--;
		if (!len)
		{
			printf("Error\nThe map must be closed by walls\n");
			return (0);
		}
		if (len != (size_t)map->width)
		{
			printf("Error\nThe map must be rectangular\n");
			return (0);
		}
		if (map->contents[i][0] != '1' || map->contents[i][map->width - 1] != '1'
			)//|| ((i == 0 || i == map->height - 1) && !check_if_string_have_one_unique_char(map->contents[i], '1')))
		{
			printf("%d %s %d\n", i, map->contents[i], map->width);
			printf("Error\nThe map must be closed by walls\n");
			return (0);
		}
		int j = 0;
		while (j < map->width)
		{
			if (!ft_strchr("01CEPX", map->contents[i][j]))
			{
				printf("Error\nThe map can be composed of only these 5 characters: '0', '1', 'C', 'E', 'P'\n");
				return (0);
			}
			start += (map->contents[i][j] == 'P');
			end += (map->contents[i][j] == 'E');
			collectible += (map->contents[i][j] == 'C');
			j++;
		}
		i++;
	}
	if (end != 1 || start != 1 || collectible < 1)
	{
		printf("Error\nThe map must contain 1 exit, at least 1 collectible, and 1 starting position to be valid.\n");
		return (0);
	}
	map->collectibles_count = collectible;
	//if (!check_map_for_valid_path(map))
	//	return (0);
	return (1);
}

char	*get_next_line(int fd)
{
	char	*s;
	char	c[2];
	int		ret;
	char	*tmp;

	s = 0;
	c[1] = 0;
	while (1)
	{
		ret = read(fd, &c[0], 1);
		if (ret < 0)
		{
			free(s);
			return (0); // todo
		}
		else if (!ret)
			break ;
		tmp = ft_strjoin(s, c);
		free(s);
		s = tmp;
		if (c[0] == '\n')
			break ;
	}
	return (s);
}

int	parse_map(t_map *map, char *map_filename)
{
	int fd = open(map_filename, O_RDONLY);
	if (fd < 0)
	{
		//todo: use perror
		printf("Error\nread failed\n");
		return (0);
	}
	while (1)
	{
		char *line = get_next_line(fd);
		if (!line)
			break ;
		size_t len = ft_strlen(line);
		if (len > 1 && line[len - 1] == '\n')
			len--;
		if (!map->width)
			map->width = len;
		map->contents = realloc(map->contents, (map->height + 1) * sizeof(char *));
		if (!map->contents)
		{
			perror("realloc");
			return (0);
		}
		map->contents[map->height] = line;
		map->height++;
	}
	if (!check_if_map_is_valid(map))
		return (0);
	if (close(fd) < 0)
	{
		perror("Error\n");
		return (0);
	}
	return (1);
}
