/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 18:18:47 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 17:59:10 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	follow_player(t_game *game, t_enemy *e)
{
	e->dx = 0;
	if (game->player_x > e->x)
		e->dx = 1;
	else if (game->player_x < e->x)
		e->dx = -1;
	e->dy = 0;
	if (game->player_y > e->y)
		e->dy = 1;
	else if (game->player_y < e->y)
		e->dy = -1;
	if (!e->follow_dir && (e->dx || fabsf(e->visual_x - e->x) > 0.05f))
		e->dy = 0;
	if (e->follow_dir && (e->dy || fabsf(e->visual_y - e->y) > 0.05f))
		e->dx = 0;
	emit_particules(game, &(t_particule_emitter){
		.base_x = (e->visual_x + 0.5f) * game->cell_dim,
		.base_y = (e->visual_y + 0.5f) * game->cell_dim,
		.use_dir = 1,
		.dx = e->dx,
		.dy = e->dy,
		.max_lifetime = 1.f,
		.r = 1, .g = 0, .b = 0,
		.count = 2,
	});
}

void	draw_enemy(t_game *game, t_enemy *e, t_image *img)
{
	float	min_x;
	float	min_y;

	min_x = game->offset_x + e->visual_x * game->cell_dim;
	min_y = game->offset_y + e->visual_y * game->cell_dim;
	if (e->mad)
		e->t += game->dt;
	else
		e->t -= game->dt;
	if (e->t < 0)
		e->t = 0;
	if (e->t > 1)
		e->t = 1;
	add_light_circle(game, (t_light){
		.cx = min_x + 0.5f * game->cell_dim,
		.cy = min_y + 0.5f * game->cell_dim,
		.r = 5,
		.color = lerp_color(0xff9999, 0xff3333, e->t)
	});
	draw_image(game, img, min_x, min_y);
}

int	is_enemy_mad(t_game *game, t_enemy *e)
{
	if (game->player_dead)
		return (0);
	return ((e->visual_x - game->player_visual_x)
		* (e->visual_x - game->player_visual_x)
		+ (e->visual_y - game->player_visual_y)
		* (e->visual_y - game->player_visual_y)
		<= 5.1 * 5.1);
}

void	update_enemy_position(t_game *game, t_enemy *e)
{
	do_move(game, (t_move){
		.visual_p[0] = &e->visual_x,
		.visual_p[1] = &e->visual_y,
		.dx[0] = e->dx,
		.dx[1] = e->dy,
		.vel[0] = &e->vel_x,
		.vel[1] = &e->vel_y,
		.game_p[0] = &e->x,
		.game_p[1] = &e->y,
		.a[0] = e->dx * 100,
		.a[1] = e->dy * 100,
		.is_player = 0,
	});
}

void	update_and_draw_enemies(t_game *game)
{
	int		i;
	t_enemy	*e;
	t_image	*img;

	i = 0;
	while (i < game->enemies_count)
	{
		e = game->enemies + i;
		e->mad = is_enemy_mad(game, e);
		e->dx = 0;
		e->dy = 0;
		img = &game->enemy_idle[(e->frame / 6) % 4][e->dx < 0];
		if (e->mad)
		{
			follow_player(game, e);
			img = &game->enemy_run[(e->frame / 4) % 4][e->dx < 0];
		}
		update_enemy_position(game, e);
		if (e->mad
			&& ((!e->follow_dir && !e->vel_x) || (e->follow_dir && !e->vel_y)))
			e->follow_dir = !e->follow_dir;
		e->frame++;
		draw_enemy(game, e, img);
		i++;
	}
}
