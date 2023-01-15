/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 10:11:15 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 11:34:31 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void	init_offscreen_images(t_game *game)
{
	game->window_scale = 2;
	game->draw_image.width = WINDOW_WIDTH / game->window_scale;
	game->draw_image.height = WINDOW_HEIGHT / game->window_scale;
	game->draw_image.line_length = game->draw_image.width * 4;
	game->draw_image.pixels = ft_alloc(game,
			game->draw_image.line_length * game->draw_image.height);
	game->window_image.width = WINDOW_WIDTH;
	game->window_image.height = WINDOW_HEIGHT;
	game->window_image.img = mlx_new_image(game->mlx,
			WINDOW_WIDTH, WINDOW_HEIGHT);
	game->window_image.pixels = mlx_get_data_addr(game->window_image.img,
			&game->window_image.bits_per_pixel,
			&game->window_image.line_length,
			&game->window_image.endian);
	game->light_scale = 0.25f;
	game->light_image.width = game->draw_image.width * game->light_scale;
	game->light_image.height = game->draw_image.height * game->light_scale;
	game->light_image.line_length = game->light_image.width * 4;
	game->light_image.pixels = ft_alloc(game,
			game->light_image.line_length * game->light_image.height);
}

void	calc_cell_dim_and_offset(t_game *game)
{
	int	c1;
	int	c2;

	c1 = game->draw_image.width / game->map.width;
	c2 = game->draw_image.height / game->map.height;
	game->cell_dim = c1;
	if (c2 < c1)
		game->cell_dim = c2;
	game->offset_x = (game->draw_image.width
			- game->cell_dim * game->map.width) / 2;
	game->offset_y = (game->draw_image.height
			- game->cell_dim * game->map.height) / 2;
}

void	init_mlx_and_create_window(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
	{
		printf("Error\nfailed to init mlx\n");
		exit_game(game, 1);
	}
	game->window = mlx_new_window(game->mlx,
			WINDOW_WIDTH, WINDOW_HEIGHT, "so_long");
}

void	init_game(t_game *game, char *map_file)
{
	ft_memset(game, 0, sizeof(*game));
	if (!parse_map(&game->original_map, map_file))
		exit_game(game, 1);
	restart_game(game);
	init_mlx_and_create_window(game);
	if (!game->window)
	{
		printf("Error\nfailed to create window\n");
		exit_game(game, 1);
	}
	game->data_read_fd = open("game_data.zf", O_RDONLY);
	if (game->data_read_fd < 0)
	{
		fprintf(stderr, "Error\nfailed to open game_data.zf");
		exit_game(game, 1);
	}
	init_offscreen_images(game);
	calc_cell_dim_and_offset(game);
	load_all_images(game);
	init_background(game);
}
