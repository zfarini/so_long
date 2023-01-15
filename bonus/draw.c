/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 12:10:23 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 17:59:52 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	clamp_draw_bounds(t_game *game, t_draw_info *info)
{
	if (info->min_x < 0)
		info->min_x = 0;
	if (info->min_y < 0)
		info->min_y = 0;
	if (info->max_x > game->draw_image.width)
		info->max_x = game->draw_image.width;
	if (info->max_y > game->draw_image.height)
		info->max_y = game->draw_image.height;
}

t_draw_info	get_draw_info(t_game *game, t_image *image, int min_x, int min_y)
{
	t_draw_info	info;

	info.min_x = min_x;
	info.min_y = min_y;
	info.max_x = info.min_x + game->cell_dim;
	info.max_y = info.min_y + game->cell_dim;
	info.x_c = (1.0f / (info.max_x - info.min_x)) * image->width;
	info.y_c = (1.0f / (info.max_y - info.min_y)) * image->height;
	clamp_draw_bounds(game, &info);
	return (info);
}

void	draw_image(t_game *game, t_image *image, int min_x, int min_y)
{
	int			x;
	int			y;
	t_draw_info	info;

	info = get_draw_info(game, image, min_x, min_y);
	y = info.min_y;
	while (y < info.max_y)
	{
		x = info.min_x;
		while (x < info.max_x)
		{
			info.src = *((unsigned *)(image->pixels
						+ ((int)((y - min_y) * info.y_c)) * image->line_length
						+ ((int)((x - min_x) * info.x_c)) * 4));
			info.dest = (unsigned *)(game->draw_image.pixels
					+ y * game->draw_image.line_length
					+ x * 4);
			*info.dest = lerp_color(*info.dest, info.src,
					((info.src >> 24) & 0xFF) * ONE_OVER_255);
			x++;
		}
		y++;
	}
}

void	draw_particule(t_game *game, t_particule *p, unsigned int color)
{
	int			y;
	int			x;
	t_draw_info	info;

	info.min_x = game->offset_x + p->x - p->size * 0.5f;
	info.min_y = game->offset_y + p->y - p->size * 0.5f;
	info.max_x = info.min_x + p->size;
	info.max_y = info.min_y + p->size;
	clamp_draw_bounds(game, &info);
	y = info.min_y;
	while (y < info.max_y)
	{
		x = info.min_x;
		while (x < info.max_x)
		{
			info.dest = (unsigned int *)(game->draw_image.pixels
					+ y * game->draw_image.line_length
					+ x * 4);
			*info.dest = lerp_color(*info.dest, color,
					((color >> 24) & 0xFF) * ONE_OVER_255);
			x++;
		}
		y++;
	}
}

void	draw_death_screen(t_game *game)
{
	t_draw_info	info;

	game->dead_t += game->dt * 0.8;
	info.t = clamp(game->dead_t, 0, 1);
	info.y = 0;
	while (info.y < game->window_image.height)
	{
		info.x = 0;
		while (info.x < game->window_image.width)
		{
			info.dest = (unsigned *)(game->window_image.pixels
					+ info.y * game->window_image.line_length
					+ info.x * (game->window_image.bits_per_pixel >> 3));
			info.src = 0;
			if (info.y >= 250 && info.y < 250 + game->death_image.height
				&& info.x >= 700 && info.x < 700 + game->death_image.width)
				info.src = *((unsigned *)(game->death_image.pixels
							+ (info.y - 250) * game->death_image.line_length
							+ (info.x - 700) * 4));
			*info.dest = lerp_color(*info.dest, info.src, info.t);
			info.x++;
		}
		info.y++;
	}
}
