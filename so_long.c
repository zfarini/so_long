#include "so_long.h"

float last_frame_time = 0;
char *images[64];

void restart_game(t_game *game);

unsigned int	lerp_color(unsigned int color1, unsigned int color2, float t)
{
	t_color_rgb		c1;
	t_color_rgb		c2;
	unsigned int	result;

	c1.r = (color1 >> 16) & 0xFF;
	c1.g = (color1 >> 8) & 0xFF;
	c1.b = (color1 >> 0) & 0xFF;
	c2.r = (color2 >> 16) & 0xFF;
	c2.g = (color2 >> 8) & 0xFF;
	c2.b = (color2 >> 0) & 0xFF;
	result = ((unsigned)(c1.r + (c2.r - c1.r) * t + 0.5f) << 16)
		|((unsigned)(c1.g + (c2.g - c1.g) * t + 0.5f) << 8)
		|((unsigned)(c1.b + (c2.b - c1.b) * t + 0.5f) << 0);
	return (result);
}

float clamp(float x, float min, float max)
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
		perror("Error\nmalloc");
		exit_game(game, 1);
	}
	return (result);
}
void exit_game(t_game *game, int failed)
{
	int i;

	if (game->mlx)
		mlx_destroy_image(game->mlx, game->window_image.img);
	if (game->window)
		mlx_destroy_window(game->mlx, game->window);
	//why free here idk?
	i = 0;
	while (i < game->original_map.height)
	{
		if (game->original_map.arr)
			free(game->original_map.arr[i]);
		if (game->map.arr)
			free(game->map.arr[i]);
		i++;
	}
	free(game->original_map.arr);
	free(game->map.arr);
	free(game->background.pixels);
	free(game->draw_image.pixels);
	i = 0;
	while (i < array_length(game->images))
		free(images[i++]);
	exit(failed);
}

int on_key_down(int keycode, t_game *game)
{
	if (keycode == ESCAPE)
		exit_game(game, 0);
	if (keycode == KEY_UP)
		game->player_dy = -1, game->player_dx = 0;
	else if (keycode == KEY_LEFT) 
	{
		game->player_dir = 1;
		game->player_dx = -1, game->player_dy = 0;
	}
	else if (keycode == KEY_DOWN) 
		game->player_dy = 1, game->player_dx = 0;
	else if (keycode == KEY_RIGHT)
	{
		game->player_dir = 0;
		game->player_dx = 1, game->player_dy = 0;
	}
	return (0);
}

int on_key_up(int keycode, t_game *game)
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

int game_loop(t_game *game)
{
	clock_t last_time;
	clock_t curr_time;
	char s[256];

	last_time = clock();
	memset(game->light_image.pixels, 0, game->light_image.line_length * game->light_image.height);
	draw_map(game);
	update_and_draw_player(game);
	update_and_draw_enemies(game);
	update_and_draw_particules(game);
	add_light_and_draw_to_window_image(game);
	if (game->player_dead)
		draw_death_screen(game);
	curr_time = clock() - last_time;
	last_frame_time = ((double)curr_time / CLOCKS_PER_SEC) * 1000;
	while ((double)curr_time / CLOCKS_PER_SEC < dt)
		curr_time = clock() - last_time;
	mlx_put_image_to_window(game->mlx, game->window, game->window_image.img, 0, 0);
	sprintf(s, "moves: %d", game->moves_count);
	mlx_string_put(game->mlx, game->window, game->window_image.width - strlen(s) * 10, 0, 0xffffff, s);
	sprintf(s, "time: %.2f", last_frame_time);
	mlx_string_put(game->mlx, game->window, game->window_image.width - strlen(s) * 10, 20, 0xffffff, s);
	if (game->player_dead && game->dead_t > 1.25)
		restart_game(game);
	return (0);
}

int exit_game2(t_game *game)
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
	mlx_do_key_autorepeatoff(game.mlx);
	mlx_hook(game.window, 2, 0, on_key_down, &game);
	mlx_hook(game.window, 3, 0, on_key_up, &game);
	mlx_hook(game.window, 17, 0, exit_game2, &game);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
	return (0);
}
