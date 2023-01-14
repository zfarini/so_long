/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 18:18:47 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/14 18:25:07 by zfarini          ###   ########.fr       */
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
		e->t += dt;
	else
		e->t -= dt;
	if (e->t < 0)
		e->t = 0;
	if (e->t > 1)
		e->t = 1;
	add_light_circle(game, min_x + 0.5f * game->cell_dim,
		min_y + 0.5f * game->cell_dim, 5,
		lerp_color(0xff9999, 0xff3333, e->t));
	draw_image(&game->draw_image, img, min_x, min_y,
		min_x + game->cell_dim, min_y + game->cell_dim);
}

int	is_enemy_mad(t_game *game, t_enemy *e)
{
	if (game->player_dead)
		return (0);
	return (dist_sq(e->visual_x + 0.5f,
			e->visual_y + 0.5f,
			game->player_visual_x + 0.5f,
			game->player_visual_y + 0.5f) <= 5.1 * 5.1);
}

void	update_enemy_position(t_game *game, t_enemy *e)
{
	update_dir(game, &e->visual_x, e->dx,
		&e->vel_x, &e->x, e->y, 1, e->dx * 100, 0);
	update_dir(game, &e->visual_y, e->dy,
		&e->vel_y, &e->y, e->x, 0, e->dy * 100, 0);
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
