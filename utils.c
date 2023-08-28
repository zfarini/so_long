/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 15:10:08 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/16 13:21:51 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

unsigned int	lerp_color(unsigned int color1, unsigned int color2, float t)
{
	t_color_rgb		c1;
	t_color_rgb		c2;
	unsigned int	result;

	c1.r = (color1 >> 16) & 0xFF;
	c1.g = (color1 >> 8) & 0xFF;
	c1.b = color1 & 0xFF;
	c2.r = (color2 >> 16) & 0xFF;
	c2.g = (color2 >> 8) & 0xFF;
	c2.b = color2 & 0xFF;
	result = ((unsigned)(c1.r + (c2.r - c1.r) * t + 0.5f) << 16)
		|((unsigned)(c1.g + (c2.g - c1.g) * t + 0.5f) << 8)
		|((unsigned)(c1.b + (c2.b - c1.b) * t + 0.5f));
	return (result);
}

float	clamp(float x, float min, float max)
{
	if (x < min)
		x = min;
	if (x > max)
		x = max;
	return (x);
}

void	*ft_alloc(t_game *game, size_t size)
{
	void	*result;

	result = ft_calloc(size, 1);
	if (!result)
	{
		ft_putstr_fd("Error\nmalloc failed\n",
			STDERR_FILENO);
		exit_game(game, 1);
	}
	return (result);
}

void	exit_game(t_game *game, int failed)
{
	int	i;

	if (game->window_image.img)
		mlx_destroy_image(game->mlx, game->window_image.img);
	if (game->window)
		mlx_destroy_window(game->mlx, game->window);
	i = -1;
	while (++i < game->original_map.height)
	{
		if (game->original_map.arr)
			free(game->original_map.arr[i]);
		if (game->map.arr)
			free(game->map.arr[i]);
	}
	free(game->original_map.arr);
	free(game->map.arr);
	free(game->background.pixels);
	free(game->draw_image.pixels);
	free(game->light_image.pixels);
	free(game->floors);
	i = 0;
	while (i < game->images_count)
		free(game->images_pixels[i++]);
	close(game->data_read_fd);
	exit(failed);
}

void	get_move_count(t_game *game, char *s)
{
	char	*num;

	ft_memcpy(s, "moves: ", 7);
	s[7] = 0;
	num = ft_itoa(game->moves_count);
	if (!num)
	{
		ft_putstr_fd("Error\nmalloc failed\n",
			STDERR_FILENO);
		exit_game(game, 1);
	}
	ft_strlcat(s, num, 32);
	free(num);
}
