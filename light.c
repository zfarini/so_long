/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 13:58:46 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 16:25:49 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

static t_draw_info	get_draw_info(t_game *game, t_light *light)
{
	t_draw_info	info;

	light->cx /= 4;
	light->cy /= 4;
	light->r *= (1.0f / 4) * game->cell_dim;
	info.min_x = light->cx - light->r;
	info.min_y = light->cy - light->r;
	info.max_x = light->cx + light->r;
	info.max_y = light->cy + light->r;
	if (info.min_x < 0)
		info.min_x = 0;
	if (info.min_y < 0)
		info.min_y = 0;
	if (info.max_x >= game->light_image.width)
		info.max_x = game->light_image.width;
	if (info.max_y >= game->light_image.height)
		info.max_y = game->light_image.height;
	return (info);
}

void	add_light_circle(t_game *game, t_light light)
{
	t_draw_info	info;

	info = get_draw_info(game, &light);
	info.y = info.min_y;
	while (info.y < info.max_y)
	{
		info.x = info.min_x;
		while (info.x < info.max_x)
		{
			info.dist_sq = (info.x - light.cx) * (info.x - light.cx)
				+ (info.y - light.cy) * (info.y - light.cy);
			if (info.dist_sq <= light.r * light.r)
			{
				info.t = 1 - sqrtf(info.dist_sq) / light.r;
				if (info.t > 1)
					info.t = 1;
				info.dest = (unsigned *)(game->light_image.pixels
						+ info.y * game->light_image.line_length
						+ info.x * 4);
				*info.dest = lerp_color(*info.dest, light.color, info.t);
			}
			info.x++;
		}
		info.y++;
	}
}

static unsigned int	get_color(t_game *game, int x, int y)
{
	unsigned int	c;
	unsigned int	p;
	unsigned int	result;

	c = *((unsigned *)(game->light_image.pixels
				+ (y >> 2) * game->light_image.line_length
				+ (x >> 2) * 4));
	p = *((unsigned *)(game->draw_image.pixels
				+ y * game->draw_image.line_length
				+ x * 4));
	result = (((int)(((p >> 16) & 0xFF)
					* (((c >> 16) & 0xFF) * ONE_OVER_255)) << 16));
	result |= (((int)(((p >> 8) & 0xFF)
					* (((c >> 8) & 0xFF) * ONE_OVER_255)) << 8));
	result |= (((int)((p & 0xFF)
					* ((c & 0xFF) * ONE_OVER_255))));
	return (result);
}

void	add_light_and_draw_to_window_image(t_game *game)
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
