/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 11:44:28 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 16:51:51 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

typedef struct s_draw_position {
	int	min_x;
	int	min_y;
	int	center_x;
	int	center_y;
}	t_draw_position;

void	draw_door(t_game *game, t_draw_position p, int x, int y)
{
	t_image	*img;

	if (game->collected_count < game->map.collectibles_count)
		img = &game->door[0];
	else
	{
		img = &game->door[1];
		emit_particules(game, &(t_particule_emitter){
			.base_x = (x + 0.5f) * game->cell_dim,
			.base_y = (y + 0.5f) * game->cell_dim,
			.max_lifetime = 1.0f,
			.r = 1, .g = 1, .b = 1,
			.normalize_dir = 1,
			.count = 3,
		});
	}
	add_light_circle(game, (t_light){
		.cx = p.center_x,
		.cy = p.center_y,
		.r = 8,
		.color = 0xffffffff
	});
	draw_image(game, img, p.min_x, p.min_y);
}

void	draw_coin(t_game *game, t_draw_position p, int x, int y)
{
	emit_particules(game, &(t_particule_emitter){
		.base_x = (x + 0.5f) * game->cell_dim,
		.base_y = (y + 0.5f) * game->cell_dim,
		.max_lifetime = 0.35f,
		.r = 1, .g = 1, .b = 0,
		.normalize_dir = 1,
		.count = 1,
	});
	add_light_circle(game, (t_light){
		.cx = p.center_x,
		.cy = p.center_y,
		.r = 4,
		.color = 0xffffff11
	});
	draw_image(game, &game->coin[(game->coin_frame / 4) % 4],
		p.min_x, p.min_y);
}

void	draw_torch(t_game *game, t_draw_position p, int x, int y)
{
	p.min_x += game->cell_dim * 0.5f;
	p.min_y += game->cell_dim * 0.1f;
	if (y == game->map.height - 2 && (x == 0 || x == game->map.width - 2))
		p.min_y += game->cell_dim * .25f;
	if (x == game->map.width - 2 && (y == 0 || y == game->map.height - 2))
		p.min_x += game->cell_dim * 0.15f;
	if (x == 0 && (y == 0 || y == game->map.height - 2))
		p.min_x -= game->cell_dim * 0.1f;
	emit_particules(game, &(t_particule_emitter){
		.base_x = (x + 0.5f) * game->cell_dim,
		.base_y = (y + 0.5f) * game->cell_dim,
		.max_lifetime = .75f,
		.r = 1, .g = .64, .b = 0,
		.count = 1,
	});
	add_light_circle(game, (t_light){
		.cx = p.center_x,
		.cy = p.center_y,
		.r = 5,
		.color = 0xffffa501
	});
	draw_image(game, &game->torch[(game->torch_frame / 4) % 4],
		p.min_x, p.min_y);
}

void	draw_cell(t_game *game, int x, int y)
{
	t_draw_position	p;

	p.min_x = game->offset_x + x * game->cell_dim;
	p.min_y = game->offset_y + y * game->cell_dim;
	p.center_x = p.min_x + 0.5f * game->cell_dim;
	p.center_y = p.min_y + 0.5f * game->cell_dim;
	if (game->map.arr[y][x] == 'E')
		draw_door(game, p, x, y);
	else if (game->map.arr[y][x] == 'C')
		draw_coin(game, p, x, y);
	else if ((!x && (!y || y == game->map.height - 2))
		|| (x == game->map.width - 2
			&& (!y || y == game->map.height - 2)))
		draw_torch(game, p, x, y);
}

void	draw_map(t_game *game)
{
	int				x;
	int				y;

	memcpy(game->draw_image.pixels, game->background.pixels,
		game->draw_image.line_length * game->draw_image.height);
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			draw_cell(game, x, y);
			x++;
		}
		y++;
	}
	game->torch_frame++;
	game->coin_frame++;
}
