/*Ideas:
-maybe dark soul theme?
-when you pick up all collectible something happend and the exit opens
-use "you die" from dark souls when you lose

TODO: should we calculate min_path_length and cause that seems too hard
TODO: 
	.
	E
	P
	is this map valid?
bug: collectible color doesn't disseapar
*/
#include "so_long.h"
#include <math.h>

int exit_game(t_game_state *game_state)
{
	mlx_destroy_image(game_state->mlx, game_state->draw_image.img);
	mlx_destroy_window(game_state->mlx, game_state->window);
	exit(0);
	return (0);
}

int on_key_down(int keycode, t_game_state *game_state)
{
	int next_row, next_col;

	next_row = game_state->player_row;
	next_col = game_state->player_col;

	if (keycode == 13) //'W'
		next_row--;
	else if (keycode == 0) // 'A'
		next_col--;
	else if (keycode == 1) // 'S'
		next_row++;
	else if (keycode == 2)
		next_col++;
	else
	{
		return (0);
	}
	printf("pressed %d\n", keycode);
	if (game_state->map.contents[next_row][next_col] == '1')
		return (0);
	if (game_state->map.contents[next_row][next_col] == 'E'
		&& game_state->collected_count < game_state->map.collectibles_count)
	{
		return (0);
	}
	if (game_state->map.contents[next_row][next_col] == 'C')
		game_state->collected_count++;
	
	game_state->map.contents[game_state->player_row][game_state->player_col] = '0';
	game_state->moves_count++;
	printf("%d %d\n", game_state->moves_count, game_state->collected_count);
	if (game_state->map.contents[next_row][next_col] == 'E')
	{
		printf("congrats\n");
		exit_game(game_state);
		return (0);
	}
	game_state->map.contents[next_row][next_col] = 'P';
	game_state->player_row = next_row;
	game_state->player_col = next_col;
	
	return (0);
}

void draw_rect(t_image *image, int min_x, int min_y, int max_x, int max_y, unsigned int color)
{
	int		y;
	int		x;
	unsigned int	*pixel;

	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	if (max_x > image->width)
		max_x = image->width;
	if (max_y > image->height)
		max_y = image->height;
	
	y = min_y;
	while (y < max_y)
	{
		x = min_x;
		while (x < max_x)
		{
			pixel = (unsigned int *)(image->pixels + y * image->line_length +
				x * (image->bits_per_pixels / 8));
			*pixel = color;
			x++;
		}
		y++;
	}
}

void draw_rect_outline(t_image *image, int min_x, int min_y, int max_x, int max_y, 
						int thickness, unsigned int color)
{
	draw_rect(image, min_x, min_y, max_x, min_y + thickness, color); // bottom
	draw_rect(image, min_x, max_y - thickness, max_x, max_y, color); // top
	draw_rect(image, min_x, min_y, min_x + thickness, max_y, color); // left
	draw_rect(image, max_x - thickness, min_y, max_x, max_y, color); // right
}

typedef unsigned int uint;

void draw_image(t_image *draw_image, t_image *image, int min_x, int min_y, int max_x, int max_y)
{

	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			
			float tx = (float)(x - min_x) / (max_x - min_x);
			float ty = (float)(y - min_y) / (max_y - min_y);
			int ix = floorf(tx * image->width);
			int iy = floorf(ty * image->height);
			uint *src =  (uint *)(image->pixels + iy * image->line_length +
			ix * image->bits_per_pixels / 8);

			uint  *dest = (uint *)(draw_image->pixels + y * draw_image->line_length +
				x * (draw_image->bits_per_pixels / 8));
			assert(!(*dest));
			//xpm doesn't have alpha???
			// int src_r = (*src >> 16) & 0xFF, dest_r = (*dest >> 16) & 0xFF;
			// int src_g = (*src >> 8) & 0xFF, dest_g = (*dest >> 8) & 0xFF;
			// int src_b = (*src >> 0) & 0xFF, dest_b = (*dest >> 0) & 0xFF;
			// float t = ((*src >> 24) & 0xFF) / 255.0f;
			// t = 1;
			//  uint r = roundf(0 + (src_r - 0) * t);
			//  uint g = roundf(0 + (src_g - 0) * t);
			//  uint b = roundf(0 + (src_b - 0) * t);
			// uint color = (r << 16) | (g << 8) | b;
			// printf("%d %d %d %f\n", r, g, b, t);
			*dest = *src;
		}
	}

}

int loop_hook(t_game_state *game_state)
{
	static int frame = 0;
	draw_rect(&game_state->draw_image, 0, 0, game_state->width, game_state->height, 0);

	for (int y = 0; y < game_state->map.height; y++)
	{
		for (int x = 0; x < game_state->map.width; x++)
		{
			
			
			char c = game_state->map.contents[y][x];
			printf("%c", c);
			int min_x = x * game_state->cell_width;
			int min_y = y * game_state->cell_height;
			int max_x = min_x + game_state->cell_width;
			int max_y = min_y + game_state->cell_height;
			int color = 0;
			if (c == '1')
				color = 0x000000ff;
			else if (c == 'P')
			{
				draw_image(&game_state->draw_image, &game_state->player_image, min_x, min_y, max_x, max_y);
				continue;
			}
			else if (c == 'E')
				color = (game_state->collected_count == game_state->map.collectibles_count ? 0x00ff0000 : 0x00ff00ff);
			else if (c == 'C')
				color = 0x005f0f0f;
			draw_rect(&game_state->draw_image, min_x, min_y, max_x, max_y, color);
			
			//draw_rect_outline(&game_state->draw_image, min_x, min_y, max_x, max_y, 1, 0x00ff00ff);
		}
		printf("\n");
	}
	printf("\n%d\n", frame++);
	mlx_put_image_to_window(game_state->mlx, game_state->window, game_state->draw_image.img, 0, 0);
	return (0);
}

int main(int argc, char **argv)
{
	t_game_state	game_state = {0};

	if (argc != 2)
	{
		printf("Error\nExpected 1 argument\n");
		return (1);
	}
	//todo: trim the string? ".ber?"
	if (!parse_map(&game_state.map, argv[1]))
		return (1);

	game_state.mlx = mlx_init();
	//find these using map width and height
	game_state.cell_height = 32;//I can make these fixed
	game_state.cell_width = 32;
	game_state.width = game_state.cell_width * game_state.map.width;
	game_state.height = game_state.cell_height * game_state.map.height;
	game_state.draw_image.width = game_state.width;
	game_state.draw_image.height = game_state.height;
	game_state.window = mlx_new_window(game_state.mlx, game_state.draw_image.width, game_state.draw_image.height, "so_long");
	game_state.draw_image.img = mlx_new_image(game_state.mlx, game_state.width, game_state.height);
	game_state.draw_image.pixels = mlx_get_data_addr(game_state.draw_image.img, 
		&game_state.draw_image.bits_per_pixels, 
		&game_state.draw_image.line_length, 
		&game_state.draw_image.endian);
	for (int y = 0; y < game_state.map.height; y++)
	{
		for (int x = 0; x < game_state.map.width; x++)
		{
			if (game_state.map.contents[y][x] == 'P')
				game_state.player_row = y, game_state.player_col = x;
		}
	}
	int w, h;
	game_state.player_image.img = mlx_xpm_file_to_image(game_state.mlx, "Idle.xpm", &game_state.player_image.width, &game_state.player_image.height);
	game_state.player_image.pixels = mlx_get_data_addr(game_state.player_image.img, 
	 	&game_state.player_image.bits_per_pixels, 
	 	&game_state.player_image.line_length, 
	 	&game_state.player_image.endian);
	
	assert(game_state.player_image.pixels && game_state.draw_image.img);
	mlx_hook(game_state.window, 2, 0, on_key_down, &game_state);
	mlx_hook(game_state.window, 17, 0, exit_game, &game_state);
	mlx_loop_hook(game_state.mlx, loop_hook, &game_state);
	mlx_loop(game_state.mlx);
}