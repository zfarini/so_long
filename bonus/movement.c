/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 18:57:54 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 17:59:42 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	move_to_closest_cell(t_game *game, t_move *move, int d)
{
	(void)game;
	if (move->delta[d] > 0)
	{
		if (*move->visual_p[d] - floorf(*move->visual_p[d]) < 0.2f)
			move->target = floorf(*move->visual_p[d]);
		else
			move->target = ceilf(*move->visual_p[d]);
	}
	else
	{
		if (ceilf(*move->visual_p[d]) - *move->visual_p[d] < 0.2f)
			move->target = ceilf(*move->visual_p[d]);
		else
			move->target = floorf(*move->visual_p[d]);
	}
	move->dir = 1;
	if (move->target - (*move->visual_p[d]) < 0)
		move->dir = -1;
	*move->visual_p[d] += move->dir * game->dt * 3;
	if ((move->dir > 0 && *move->visual_p[d] >= move->target)
		|| (move->dir < 0 && *move->visual_p[d] <= move->target))
	{
		*move->vel[d] = 0;
		*move->visual_p[d] = move->target;
	}
}

void	check_for_collision(t_game *game, t_move *move, int d)
{
	char	c1;
	char	c2;

	if (!d)
	{
		c1 = game->map.arr[*move->game_p[1]][(int)*move->visual_p[d]];
		c2 = game->map.arr[*move->game_p[1]][(int)(*move->visual_p[d] + 1)];
	}
	else
	{
		c1 = game->map.arr[(int)*move->visual_p[d]][*move->game_p[0]];
		c2 = game->map.arr[(int)(*move->visual_p[d] + 1)][*move->game_p[0]];
	}
	if ((c1 == '1' || c2 == '1') || (!move->is_player
			&& ((c1 == 'X' && (int)(*move->visual_p[d]) != *move->game_p[d])
			|| (c2 == 'X' && (int)(*move->visual_p[d] + 1) != *move->game_p[d])
		|| (c1 == 'E' || c2 == 'E' || c1 == 'C' || c2 == 'C')))
		|| (move->is_player && (c1 == 'E' || c2 == 'E')
			&& game->collected_count < game->map.collectibles_count))
	{
		*move->visual_p[d] = *move->game_p[d];
		*move->vel[d] = 0;
	}
}

void	update_position(t_game *game, t_move *move, int d)
{
	char	*curr;

	curr = &game->map.arr[*move->game_p[1]][*move->game_p[0]];
	*move->game_p[d] = roundf(*move->visual_p[d]);
	*curr = '0';
	curr = &game->map.arr[*move->game_p[1]][*move->game_p[0]];
	if ((move->is_player && *curr == 'X') || (!move->is_player && *curr == 'P'))
		game->player_dead = 1;
	if (move->is_player && *curr == 'C')
		game->collected_count++;
	if (move->is_player && *curr == 'E')
		exit_game(game, 0);
	if (move->is_player)
		*curr = 'P';
	else
		*curr = 'X';
}

void	do_move_dir(t_game *game, t_move *move, int d)
{
	move->a[d] -= (*move->vel[d]) * 20;
	move->delta[d] = 0.5f * game->dt * game->dt * move->a[d]
		+ game->dt * (*move->vel[d]);
	*move->visual_p[d] += move->delta[d];
	*move->vel[d] += move->a[d] * game->dt;
	if (!move->dx[d] && fabsf(move->delta[d]) < 0.2f)
		move_to_closest_cell(game, move, d);
	if (move->is_player && roundf(*move->visual_p[d]) != *move->game_p[d])
		game->moves_count++;
	if (move->is_player && move->dx[d])
		game->player_running = 1;
	check_for_collision(game, move, d);
	update_position(game, move, d);
}

void	do_move(t_game *game, t_move move)
{
	do_move_dir(game, &move, 0);
	do_move_dir(game, &move, 1);
}
