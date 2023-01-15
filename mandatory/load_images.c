/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_images.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 11:24:12 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 15:20:43 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

t_image	load_next_image(t_game *game)
{
	t_image	img;

	if (read(game->data_read_fd, &img.width, sizeof(img.width))
		!= sizeof(img.width))
	{
		fprintf(stderr, "Error\nfailed to read from game_data.zf");
		exit_game(game, 1);
	}
	if (read(game->data_read_fd, &img.height, sizeof(img.height))
		!= sizeof(img.height))
	{
		fprintf(stderr, "Error\nfailed to read from game_data.zf");
		exit_game(game, 1);
	}
	img.line_length = img.width * sizeof(unsigned int);
	img.pixels = ft_alloc(game, img.height * img.line_length);
	if (read(game->data_read_fd, img.pixels, img.height * img.line_length)
		!= img.height * img.line_length)
	{
		fprintf(stderr, "Error\nfailed to read from game_data.zf");
		exit_game(game, 1);
	}
	game->images_pixels[game->images_count++] = img.pixels;
	return (img);
}

t_image	flip_image_by_x(t_game *game, t_image *image)
{
	t_image	res;
	int		y;
	int		x;

	res = *image;
	res.pixels = ft_alloc(game, res.height * res.line_length);
	y = 0;
	while (y < res.height)
	{
		x = 0;
		while (x < res.width)
		{
			*((unsigned int *)(res.pixels + y * res.line_length + x * 4))
				= *((unsigned int *)(image->pixels + y * image->line_length
						+ (image->width - x - 1) * 4));
			x++;
		}
		y++;
	}
	return (res);
}

void	load_player_images(t_game *game)
{
	int		i;

	i = 0;
	while (i < 4)
	{
		game->player_idle[i][0] = load_next_image(game);
		game->player_run[i][0] = load_next_image(game);
		game->player_run[i][0].height -= 7;
		game->player_run[i][0].pixels += 7
			* game->player_idle[i][0].line_length;
		game->player_idle[i][0].height -= 7;
		game->player_idle[i][0].pixels += 7
			* game->player_idle[i][0].line_length;
		game->player_idle[i][1] = flip_image_by_x(game,
				&game->player_idle[i][0]);
		game->player_run[i][1] = flip_image_by_x(game,
				&game->player_run[i][0]);
		i++;
	}
}

void	load_enemy_images(t_game *game)
{
	int		i;

	i = 0;
	while (i < 4)
	{
		game->enemy_idle[i][0] = load_next_image(game);
		game->enemy_run[i][0] = load_next_image(game);
		game->enemy_run[i][0].height -= 3;
		game->enemy_run[i][0].pixels += 3 * game->enemy_idle[i][0].line_length;
		game->enemy_idle[i][0].height -= 3;
		game->enemy_idle[i][0].pixels += 3
			* game->enemy_idle[i][0].line_length;
		game->enemy_idle[i][1] = flip_image_by_x(game, &game->enemy_idle[i][0]);
		game->enemy_run[i][1] = flip_image_by_x(game, &game->enemy_run[i][0]);
		i++;
	}
}

void	load_all_images(t_game *game)
{
	int		i;

	load_player_images(game);
	load_enemy_images(game);
	i = 0;
	while (i < 8)
	{
		game->floor[i] = load_next_image(game);
		if (i < 4)
		{
			game->coin[i] = load_next_image(game);
			game->torch[i] = load_next_image(game);
		}
		i++;
	}
	game->hole = load_next_image(game);
	game->door[0] = load_next_image(game);
	game->door[1] = load_next_image(game);
	game->wall_top = load_next_image(game);
	game->wall_bottom = load_next_image(game);
	game->wall_left = load_next_image(game);
	game->wall_right = load_next_image(game);
	game->floor_ladder = load_next_image(game);
	game->death_image = load_next_image(game);
}
