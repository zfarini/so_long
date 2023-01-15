/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_restart.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 11:33:49 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 11:33:55 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	init_game_values(t_game *game)
{
	int	i;

	game->particule_count = 0;
	game->moves_count = 0;
	game->collected_count = 0;
	game->player_frame = 0;
	game->player_running = 0;
	game->player_dx = 0;
	game->player_dx = 0;
	game->player_vel_x = 0;
	game->player_vel_y = 0;
	game->coin_frame = 0;
	game->dead_t = 0;
	game->player_dead = 0;
	game->torch_frame = 0;
	game->enemies_count = 0;
	game->map = game->original_map;
	game->map.arr = ft_alloc(game, game->map.height * sizeof(char *));
	i = 0;
	while (i < game->map.height)
	{
		game->map.arr[i] = ft_strdup(game->original_map.arr[i]);
		i++;
	}
}

void	alloc_enemies(t_game *game)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (game->map.arr[y][x] == 'X')
				game->enemies_count++;
			if (game->map.arr[y][x] == 'P')
			{
				game->player_visual_y = y;
				game->player_y = y;
				game->player_visual_x = x;
				game->player_x = x;
			}
			x++;
		}
		y++;
	}
	game->enemies = ft_alloc(game, game->enemies_count * sizeof(t_enemy));
}

void	init_enemies(t_game *game)
{
	int	x;
	int	y;
	int	e;

	y = 0;
	e = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (game->map.arr[y][x] == 'X')
			{
				game->enemies[e].visual_y = y;
				game->enemies[e].y = y;
				game->enemies[e].visual_x = x;
				game->enemies[e].x = x;
				e++;
			}
			x++;
		}
		y++;
	}
}

void	restart_game(t_game *game)
{
	int	i;

	if (game->map.arr)
	{
		i = 0;
		while (i < game->map.height)
			free(game->map.arr[i++]);
		free(game->map.arr);
		free(game->enemies);
	}
	init_game_values(game);
	alloc_enemies(game);
	init_enemies(game);
}
