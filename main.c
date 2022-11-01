#include <mlx.h>
#include <assert.h>
#include <stdio.h>
#include "libft/libft.h"
#include "get_next_line/get_next_line.h"

typedef struct s_map {
	int		width;
	int		height;
	char	**contents;
}	t_map;

typedef struct s_game_state {
	void	*mlx;
	void	*window;
	int		width;
	int		height;
	void	*image;
	char	*pixels;
	int		bits_per_pixels;
	int		line_length;
	int		endian;
	t_map	map;
}	t_game_state;

int exit_game(t_game_state *game_state)
{
	mlx_destroy_image(game_state->mlx, game_state->image);
	mlx_destroy_window(game_state->mlx, game_state->window);
	exit(0);
}

int expose_hook(t_game_state *game_state)
{
	printf("expose_hook\n");
}

int key_hook(int keycode,t_game_state *game_state)
{
	if (keycode == 53)
		exit_game(game_state);
	printf("key_hook: %d\n", keycode);
}
int mouse_hook(int button, int x, int y, t_game_state *game_state)
{
	printf("mouse_hook: %d %d %d\n", button, x, y);
}

int loop_hook(t_game_state *game_state)
{
	//printf("loop hook %d %d\n", game_state->line_length, game_state->bits_per_pixels);
	for (int y = 0; y < game_state->height; y++)
	{
		for (int x = 0; x < game_state->width; x++)
		{
			char  *pixel = game_state->pixels + y * game_state->line_length + 
				x * (game_state->bits_per_pixels / 8);
			//*pixel = 0x00ff0000;
			 *pixel++ = 0xff;//B
			 *pixel++ = 0;//G
			 *pixel++ = 0;//R
			//mlx_pixel_put(game_state->mlx, game_state->window, x, y, 0x00ff0000);
		}
	}
	
	mlx_put_image_to_window(game_state->mlx, game_state->window, game_state->image, 0, 0);
	return (1);
}

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
			if (!ft_strchr("01CEP", map->contents[i][j]))
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
	return (1);
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
			perror("Error\n");
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

int main(int argc, char **argv)
{
	t_game_state	game_state;

	if (argc != 2)
	{
		printf("Error\nExpected 1 argument\n");
		return (1);
	}
	//todo: trim the string? ".ber?"
	if (!parse_map(&game_state.map, argv[1]))
		return (1);

	game_state.mlx = mlx_init();
	game_state.width = 960;
	game_state.height = 540;
	game_state.window = mlx_new_window(game_state.mlx, game_state.width, game_state.height, "so_long");
	game_state.image = mlx_new_image(game_state.mlx, game_state.width, game_state.height);
	game_state.pixels = mlx_get_data_addr(game_state.image, &game_state.bits_per_pixels,
		&game_state.line_length, &game_state.endian);
	
	mlx_key_hook(game_state.window, key_hook, &game_state);
	//mlx_expose_hook(game_state.window, expose_hook, &game_state);
	//mlx_mouse_hook(game_state.window, mouse_hook, &game_state);
	//p(&game_state);

	mlx_hook(game_state.window, 17, 0, exit_game, &game_state);
	mlx_loop_hook(game_state.mlx, loop_hook, &game_state);
	mlx_loop(game_state.mlx);
}