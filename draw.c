/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 12:10:23 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 19:10:38 by zfarini          ###   ########.fr       */
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

static unsigned int	get_color(t_game *game, int x, int y)
{
	unsigned int	p;

	p = *((unsigned *)(game->draw_image.pixels
				+ y * game->draw_image.line_length
				+ x * 4));
	return (p);
}

void	draw_to_window_image(t_game *game)
{
	t_draw_info	info;

	info.y = 0;
	while (info.y < game->draw_image.height)
	{
		info.x = 0;
		while (info.x < game->draw_image.width)
		{
			info.color = get_color(game, info.x, info.y);
			info.dest = (unsigned *)(game->window_image.pixels
					+ (info.y << 1) * game->window_image.line_length
					+ (info.x << 1) * (game->window_image.bits_per_pixel >> 3));
			info.dest2 = (unsigned *)((char *)info.dest
					+ game->window_image.line_length);
			*info.dest = info.color;
			*(info.dest + 1) = info.color;
			*info.dest2 = info.color;
			*(info.dest2 + 1) = info.color;
			info.x++;
		}
		info.y++;
	}
}
