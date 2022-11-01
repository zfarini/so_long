#include "so_long.h"

int exit_game(t_game_state *game_state)
{
	mlx_destroy_image(game_state->mlx, game_state->image);
	mlx_destroy_window(game_state->mlx, game_state->window);
	exit(0);
}

int expose_hook(t_game_state *game_state)
{
	printf("expose_hook\n");
}

int key_hook(int keycode,t_game_state *game_state)
{
	if (keycode == 53)
		exit_game(game_state);
	printf("key_hook: %d\n", keycode);
}
int mouse_hook(int button, int x, int y, t_game_state *game_state)
{
	printf("mouse_hook: %d %d %d\n", button, x, y);
}

int loop_hook(t_game_state *game_state)
{
	//printf("loop hook %d %d\n", game_state->line_length, game_state->bits_per_pixels);
	for (int y = 0; y < game_state->height; y++)
	{
		for (int x = 0; x < game_state->width; x++)
		{
			char  *pixel = game_state->pixels + y * game_state->line_length + 
				x * (game_state->bits_per_pixels / 8);
			//*pixel = 0x00ff0000;
			 *pixel++ = 0xff;//B
			 *pixel++ = 0;//G
			 *pixel++ = 0;//R
			//mlx_pixel_put(game_state->mlx, game_state->window, x, y, 0x00ff0000);
		}
	}
	
	mlx_put_image_to_window(game_state->mlx, game_state->window, game_state->image, 0, 0);
	return (1);
}

int main(int argc, char **argv)
{
	t_game_state	game_state;

	if (argc != 2)
	{
		printf("Error\nExpected 1 argument\n");
		return (1);
	}
	//todo: trim the string? ".ber?"
	if (!parse_map(&game_state.map, argv[1]))
		return (1);

	game_state.mlx = mlx_init();
	game_state.width = 960;
	game_state.height = 540;
	game_state.window = mlx_new_window(game_state.mlx, game_state.width, game_state.height, "so_long");
	game_state.image = mlx_new_image(game_state.mlx, game_state.width, game_state.height);
	game_state.pixels = mlx_get_data_addr(game_state.image, &game_state.bits_per_pixels,
		&game_state.line_length, &game_state.endian);
	
	mlx_key_hook(game_state.window, key_hook, &game_state);
	//mlx_expose_hook(game_state.window, expose_hook, &game_state);
	//mlx_mouse_hook(game_state.window, mouse_hook, &game_state);
	//p(&game_state);

	mlx_hook(game_state.window, 17, 0, exit_game, &game_state);
	mlx_loop_hook(game_state.mlx, loop_hook, &game_state);
	mlx_loop(game_state.mlx);
}