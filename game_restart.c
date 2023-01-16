/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_restart.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 11:33:49 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/16 13:08:42 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	duplicate_map(t_game *game)
{
	int	i;

	game->map = game->original_map;
	game->map.arr = ft_alloc(game, game->map.height * sizeof(char *));
	i = 0;
	while (i < game->map.height)
	{
		game->map.arr[i] = ft_strdup(game->original_map.arr[i]);
		if (!game->map.arr[i])
		{
			ft_putstr_fd("Error\nmalloc failed\n", STDERR_FILENO);
			exit_game(game, 1);
		}
		i++;
	}
}

void	init_game_values(t_game *game)
{
	game->moves_count = 0;
	game->collected_count = 0;
	game->player_dx = 0;
	game->player_dx = 0;
	duplicate_map(game);
}

void	init_player(t_game *game)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (game->map.arr[y][x] == 'P')
			{
				game->player_y = y;
				game->player_x = x;
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
	}
	init_game_values(game);
	init_player(game);
}
