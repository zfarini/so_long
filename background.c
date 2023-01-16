/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   background.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 11:21:09 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/16 11:59:43 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	init_floor(t_game *game)
{
	int	y;
	int	x;
	int	p;

	game->floors = ft_alloc(game,
			game->map.width * game->map.height * sizeof(int));
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			p = (x + y * 2) % 8;
			game->floors[y * game->map.width + x] = p;
			x++;
		}
		y++;
	}
}

t_image	*find_background_wall_img(t_game *game, int x, int y)
{
	t_image	*img;

	img = 0;
	if ((!x && !y) || (x == game->map.width - 1 && !y)
		|| (!x && y == game->map.height - 1)
		|| (x == game->map.width - 1 && y == game->map.height - 1))
	{
	}
	else if (!x)
		img = &game->wall_left;
	else if (x == game->map.width - 1)
		img = &game->wall_right;
	else if (!y)
		img = &game->wall_top;
	else if (y == game->map.height - 1)
	{
		img = &game->wall_bottom;
	}
	else
		img = &game->hole;
	return (img);
}

void	init_background_cell(t_game *game, int x, int y)
{
	int		min_x;
	int		min_y;
	t_image	*img;

	min_x = game->offset_x + x * game->cell_dim;
	min_y = game->offset_y + y * game->cell_dim;
	if (game->map.arr[y][x] == '1')
		img = find_background_wall_img(game, x, y);
	else if (game->map.arr[y][x] == 'E')
		img = &game->floor_ladder;
	else
		img = &game->floor[game->floors[y * game->map.width + x]];
	if (img == &game->wall_bottom)
		min_y -= game->cell_dim * 0.75f;
	if (img)
		draw_image(game, img, min_x, min_y);
}

void	init_background(t_game *game)
{
	int	x;
	int	y;

	init_floor(game);
	game->background.width = game->draw_image.width;
	game->background.height = game->draw_image.height;
	game->background.line_length = game->background.width * 4;
	game->background.pixels = ft_alloc(game,
			game->background.line_length * game->background.height);
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			init_background_cell(game, x, y);
			x++;
		}
		y++;
	}
	ft_memcpy(game->background.pixels, game->draw_image.pixels,
		game->draw_image.line_length * game->draw_image.height);
}
