/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 18:26:10 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/15 19:16:28 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	update_player_position(t_game *game)
{
	char *new;

	new = &game->map.arr[game->player_y + game->player_dy][game->player_x + game->player_dx];
	if (*new == '1' || (*new == 'E' && game->collected_count < game->map.collectibles_count))
		return ;
	game->map.arr[game->player_y][game->player_x] = '0';	
	game->player_x += game->player_dx;
	game->player_y += game->player_dy;
	if (*new == 'C')
		game->collected_count++;
	if (*new == 'E')
		exit_game(game, 0);
	if (game->player_dx || game->player_dy)
		game->moves_count++;
	*new = 'P';
}

void	update_and_draw_player(t_game *game)
{
	t_image	*img;
	float	min_x;
	float	min_y;

	update_player_position(game);
	game->player_dx = 0;
	game->player_dy = 0;
	min_x = game->offset_x + game->player_x * game->cell_dim;
	min_y = game->offset_y + game->player_y * game->cell_dim;
	img = &game->player_idle[0][game->player_dir];
	draw_image(game, img, min_x, min_y);
}
