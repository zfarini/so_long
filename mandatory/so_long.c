/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 15:27:12 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 19:22:23 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

int	on_key_down(int keycode, t_game *game)
{
	if (keycode == ESCAPE)
		exit_game(game, 0);
	if (keycode == KEY_UP)
	{
		game->player_dy = -1;
		game->player_dx = 0;
	}
	else if (keycode == KEY_LEFT)
	{
		game->player_dir = 1;
		game->player_dx = -1;
		game->player_dy = 0;
	}
	else if (keycode == KEY_DOWN)
	{
		game->player_dy = 1;
		game->player_dx = 0;
	}
	else if (keycode == KEY_RIGHT)
	{
		game->player_dir = 0;
		game->player_dx = 1;
		game->player_dy = 0;
	}
	return (0);
}

int	on_key_up(int keycode, t_game *game)
{
	if (keycode == KEY_UP && game->player_dy == -1)
		game->player_dy = 0;
	if (keycode == KEY_LEFT && game->player_dx == -1)
		game->player_dx = 0;
	if (keycode == KEY_DOWN && game->player_dy == 1)
		game->player_dy = 0;
	if (keycode == KEY_RIGHT && game->player_dx == 1)
		game->player_dx = 0;
	return (0);
}

int	game_loop(t_game *game)
{
	draw_map(game);
	update_and_draw_player(game);
	draw_to_window_image(game);
	mlx_put_image_to_window(game->mlx, game->window,
		game->window_image.img, 0, 0);
	printf("moves count: %d\n", game->moves_count); 
	return (0);
}

int	mlx_exit_game(t_game *game)
{
	exit_game(game, 0);
	return (0);
}

int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
	{
		printf("Error\nExpected 1 argument\n");
		return (1);
	}
	srand(time(0));
	init_game(&game, argv[1]);
	mlx_do_key_autorepeaton(game.mlx);
	mlx_hook(game.window, 2, 0, on_key_down, &game);
	mlx_hook(game.window, 3, 0, on_key_up, &game);
	mlx_hook(game.window, 17, 0, mlx_exit_game, &game);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
	return (0);
}
