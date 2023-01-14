/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 18:26:10 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/14 18:36:38 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	update_player_position(t_game *game)
{
	game->player_running = 0;
	update_dir(game, &game->player_visual_x, game->player_dx,
		&game->vel_x, &game->player_x, game->player_y,
		1, game->player_dx * 200, 1);
	update_dir(game, &game->player_visual_y, game->player_dy,
		&game->vel_y, &game->player_y, game->player_x,
		0, game->player_dy * 200, 1);
}

void	emit_player_particules(t_game *game)
{
	if (game->player_dx || game->player_dy)
	{
		emit_particules(game, &(t_particule_emitter){
			.base_x = (game->player_visual_x + 0.5f) * game->cell_dim,
			.base_y = (game->player_visual_y + 0.5f) * game->cell_dim,
			.use_dir = 1,
			.dx = game->player_dx,
			.dy = game->player_dy,
			.max_lifetime = 1.f,
			.r = 0, .g = 1, .b = 1,
			.count = 3,
		});
	}
}

void	update_and_draw_player(t_game *game)
{
	t_image	*img;
	float	min_x;
	float	min_y;

	if (game->player_dead)
		return ;
	update_player_position(game);
	emit_player_particules(game);
	min_x = game->offset_x + game->player_visual_x * game->cell_dim;
	min_y = game->offset_y + game->player_visual_y * game->cell_dim;
	img = &game->player_idle[(game->player_frame / 4) % 4][game->player_dir];
	if (game->player_running)
		img = &game->player_run[(game->player_frame / 4) % 4][game->player_dir];
	game->player_frame++;
	add_light_circle(game, min_x + 0.5f * game->cell_dim,
		min_y + 0.5f * game->cell_dim, 7, 0xff55ffff);
	draw_image(&game->draw_image, img, min_x, min_y,
		min_x + game->cell_dim, min_y + game->cell_dim);
}
