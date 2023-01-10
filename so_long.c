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
#include <time.h>

#define USE_SRGB 1
int exit_game(t_game *game)
{
	mlx_destroy_image(game->mlx, game->draw_image.img);
	mlx_destroy_window(game->mlx, game->window);
	exit(0);
	return (0);
}

int last_keycode = -1;

int on_key_down(int keycode, t_game *game)
{
	last_keycode = keycode;
	return (0);
}

int on_key_up(int keycode, t_game *game)
{
	if (keycode == 13 && game->player_dy == -1)
		game->player_dy = 0;
	if (keycode == 0 && game->player_dx == -1)
		game->player_dx = 0;
	if (keycode == 1 && game->player_dy == 1)
		game->player_dy = 0;
	if (keycode == 2 && game->player_dx == 1)
		game->player_dx = 0;
	return (0);
}

void color_u32_to_rgb01(unsigned int c, float *r, float *g, float *b)
{
	*r = ((c >> 16) & 0xFF) / 255.0f;
	*g = ((c >>  8) & 0xFF) / 255.0f;
	*b = ((c >>  0) & 0xFF) / 255.0f;
}

void add_light_circle(t_image *image, int cx, int cy, int r, unsigned int color)
{
	int	min_x = cx - r;
	int min_y = cy - r;
	int max_x = cx + r;
	int max_y = cy + r;
	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	if (max_x > image->width)
		max_x = image->width;
	if (max_y > image->height)
		max_y = image->height;
	float cr, cg, cb;
	color_u32_to_rgb01(color, &cr, &cg, &cb);
	cr *= cr;
	cg *= cg;
	cb *= cb;
	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			int dist_sq = (x - cx) * (x - cx) + (y - cy) * (y - cy);
			if (dist_sq <= r * r)
			{
				float dist = sqrtf(dist_sq);
				float t = 1 - dist / r;
				t *= t;
				if (t > 1)
					t = 1;
				unsigned int *pixel = (unsigned int *)(image->pixels + y * image->line_length +
					x * (image->bits_per_pixel / 8));
				float pr, pg, pb;
				color_u32_to_rgb01(*pixel, &pr, &pg, &pb);
				//float pt = ((*pixel >> 24) & 0xFF) / 255.0f;
#if USE_SRGB
				pr *= pr;
				pg *= pg;
				pb *= pb;
#endif
#if 0
				pr = cr, pg = cg, pb = cb;
#else

				pr = pr + (cr * t - pr) * t;
				pg = pg + (cg * t - pg) * t;
				pb = pb + (cb * t - pb) * t;
#endif
#if USE_SRGB
				pr = sqrtf(pr);
				pg = sqrtf(pg);
				pb = sqrtf(pb);
#endif
				*pixel = ((unsigned)(t * 255 + 0.5f) << 24)
						|((unsigned)(pr * 255 + 0.5f) <<  16)
						|((unsigned)(pg * 255 + 0.5f) <<  8)
						|((unsigned)(pb * 255 + 0.5f) <<  0);
			}
		}
	}
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
				x * (image->bits_per_pixel / 8));
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
			ix * image->bits_per_pixel / 8);

			uint  *dest = (uint *)(draw_image->pixels + y * draw_image->line_length +
				x * (draw_image->bits_per_pixel / 8));
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

#include <string.h>

float dt = 1.0 / 60;

void update_dir(t_game *game, float *visual_x, int dx, float *vel_x, int *game_x, int game_y, int is_x)
{
	float a = dx * 250;
	a -= (*vel_x) * 20;
	float delta = 0.5f * dt * dt * a + dt * (*vel_x);
	*visual_x += delta;
	*vel_x += a * dt;
	if (!dx && fabsf(delta) < 0.05)
	{
		int dir = (delta > 0) ? 1 : -1;
		int target = (dir > 0 ? ceilf(*visual_x) : floorf(*visual_x));

		*visual_x += dir * dt * 2;
		if ((dir > 0 && *visual_x >= target)
		|| (dir < 0 && *visual_x <= target))
		{
			*vel_x = 0;
			*visual_x = target;
		}
	}
	if (is_x)
	{
		if ((game->map.contents[game_y][(int)(*visual_x)] == '1') ||
			(game->map.contents[game_y][(int)(*visual_x + 1)] == '1'))
			*visual_x = *game_x, *vel_x = 0;
		game->map.contents[game_y][*game_x] = '0';
		*game_x = roundf(*visual_x);
		game->map.contents[game_y][*game_x] = 'P';
	}
	else
	{
		if ((game->map.contents[(int)(*visual_x)][game_y]== '1') ||
			(game->map.contents[(int)(*visual_x + 1)][game_y]== '1'))
			*visual_x = *game_x, *vel_x = 0;
		game->map.contents[*game_x][game_y] = '0';
		*game_x = roundf(*visual_x);
		game->map.contents[*game_x][game_y] = 'P';
	}
}


int loop_hook(t_game *game)
{
	clock_t t = clock();
	if (last_keycode != -1)
	{
		int keycode = last_keycode; // take the last inputed move?
		if (keycode == 13) //'W'
			game->player_dy = -1, game->player_dx = 0;
		else if (keycode == 0) // 'A'
			game->player_dx = -1, game->player_dy = 0;
		else if (keycode == 1) // 'S'
			game->player_dy = 1, game->player_dx = 0;
		else if (keycode == 2)
			game->player_dx = 1, game->player_dy = 0;
		last_keycode = -1;
	}
	//if (game->player_dx || game->player_dy)
	{
		update_dir(game, &game->player_visual_x, game->player_dx, &game->vel_x, &game->player_x, game->player_y, 1);
		update_dir(game, &game->player_visual_y, game->player_dy, &game->vel_y, &game->player_y, game->player_x, 0);
	}
	if (!game->enemy_dx)
	{
		game->enemy_dx = -1;
	}

	float dx_left = abs(game->enemy_dx) * dt * 20;

	while (dx_left > 0)
	{
		float dx = 1;
		if (dx_left < 1)
			dx = dx_left;
		float new_p = game->enemy_visual_x + dx * game->enemy_dx;
		int px;

		px = new_p;
		if (game->enemy_dx > 0)
			px++;
		if (px != game->enemy_x && game->map.contents[game->enemy_y][px] != '0')
		{
			game->enemy_visual_x = game->enemy_x;
			game->enemy_dx *= -1;		
			break;
		}
		game->enemy_visual_x += dx * game->enemy_dx;

		int new = roundf(game->enemy_visual_x);
		if (new != game->enemy_x)
		{
			game->map.contents[game->enemy_y][game->enemy_x] = '0';
			game->map.contents[game->enemy_y][new] = 'X';
			game->enemy_x = new;
		}
		dx_left -= 1;
	}
	//memset(game->draw_image.pixels, 0, game->draw_image.line_length * game->draw_image.height);
	draw_rect(&game->draw_image, 0, 0, game->draw_image.width, game->draw_image.height, 0);
	for (int y = 0; y < game->light_image.height; y++)
	{
		for (int x = 0; x < game->light_image.width; x++)
		{
			unsigned int *pixel = (unsigned int *)(game->light_image.pixels + y * game->light_image.line_length +
					x * (game->light_image.bits_per_pixel / 8));
			*pixel = 0;
			*pixel = 0x222222;
		}
	}
	static float camera_x = 0;
	static float camera_y = 0;

	camera_x += (game->player_visual_x - camera_x) * 0.1f;
	camera_y += (game->player_visual_y - camera_y) * 0.1f;

	float window_center_x = game->draw_image.width * 0.5f;
	float window_center_y = game->draw_image.height * 0.5f;
	for (int dy = -20; dy < 20; dy++)
	{
		for (int dx = -20; dx < 20; dx++)
		{
			int x = (game->player_x + dx);
			int y = (game->player_y + dy);
			float center_x = window_center_x + 
					(x - camera_x) * game->cell_dim;
			float center_y = window_center_y +
					(y - camera_y) * game->cell_dim;
			float min_x = center_x - 0.5f * game->cell_dim;
			float min_y = center_y - 0.5f * game->cell_dim;
			float max_x = min_x + game->cell_dim;
			float max_y = min_y + game->cell_dim;
			if (x >= 0 && x < game->map.width && y >= 0 && y < game->map.height)
			{
				char c = game->map.contents[y][x];
				unsigned int color = 0;
				if (c == '1')
					color = 0x000000ff;
				else if (c == 'P' || c == 'X')
				{
					continue;
				}
				else if (c == 'E')
					color = (game->collected_count == game->map.collectibles_count ? 0x00ff0000 : 0x00ff00ff);
				else if (c == 'C')
					color = 0x005f0f0f;
				else
					continue;
				draw_rect(&game->draw_image, min_x, min_y, max_x, max_y, color);
			}
			//draw_rect_outline(&game->draw_image, min_x, min_y, max_x, max_y, 1, 0x0000ffff);
		}
	}
	
	for (int i = 0; i < game->particule_count;)
	{
		t_particule *p = game->particules + i;
		if (p->lifetime_left <= 0)
		{
			*p = game->particules[game->particule_count - 1];
			game->particule_count--;
			continue;
		}
		else
		{
			p->x += p->dx;
			p->y += p->dy;
			float t = p->lifetime_left / p->lifetime;
			float size = p->size;
			float min_x = window_center_x + (p->x - camera_x * game->cell_dim) - size * 0.5f;
			float min_y = window_center_y + (p->y - camera_y * game->cell_dim) - size * 0.5f;
			unsigned int color = ((unsigned)(p->r * t * 255 + 0.5f) << 16)
								|((unsigned)(p->g * t * 255 + 0.5f) <<  8)
								|((unsigned)(p->b * t * 255 + 0.5f) <<  0);

			draw_rect(&game->draw_image, min_x, min_y, min_x + size, min_y + size, color);
			i++;
		}
		p->lifetime_left -= dt;
	}
	//if (game->player_dx || game->player_dy)
	int target_dx = game->player_dx;
	int target_dy = game->player_dy;
	if (target_dx || target_dy)
	{
		for (int i = 0; i < 20; i++)
		{
			if (game->particule_count >= 4096)
				break;

			float base_x = game->player_visual_x * game->cell_dim;
			float base_y = game->player_visual_y * game->cell_dim;
			t_particule *p = &game->particules[game->particule_count++];
			if (target_dx)
			{
				p->x = base_x - target_dx * (rand() % 30);
				p->y = base_y + (rand() % 30 - 15);
				p->dx = -target_dx;
				p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
			}
			else
			{
				p->y = base_y - target_dy * (rand() % 30);
				p->x = base_x + (rand() % 30 - 15);
				p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
				p->dy = -target_dx;
			}
			p->lifetime = ((float)rand() / RAND_MAX) * 1.f;
			p->lifetime_left = p->lifetime;
			p->size = rand() % 6 + 1;
			p->r = 1;
			p->g = 1;
			p->b = 0;
		}
	}
	printf("%d\n", game->particule_count);
	float min_x, min_y;
	min_x = window_center_x + (game->player_visual_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->player_visual_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	//draw_rect(&game->draw_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim, 0xffff00);
	draw_image(&game->draw_image, &game->player_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim);

	add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffff);

	min_x = window_center_x + (game->player_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->player_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	
	min_x = window_center_x + (game->enemy_visual_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->enemy_visual_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	draw_rect(&game->draw_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim, 0x005511);
	add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 5, 0x888888);

	min_x = window_center_x + (0 - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->map.height - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffff);

	min_x = window_center_x + (game->map.width - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->map.height - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffff);
#if 1
	for (int y = 0; y < game->draw_image.height; y++)
	{
		for (int x = 0; x < game->draw_image.width; x++)
		{
			unsigned int *pixel = (unsigned int *)(game->draw_image.pixels
					+ y * game->draw_image.line_length
					+ x * (game->draw_image.bits_per_pixel / 8));
			unsigned int *c = (unsigned int *)(game->light_image.pixels
					+ y * game->light_image.line_length
					+ x * (game->light_image.bits_per_pixel / 8));

			float pr, pg, pb;
			float cr, cg, cb;
			color_u32_to_rgb01(*pixel, &pr, &pg, &pb);
			color_u32_to_rgb01(*c, &cr, &cg, &cb);
#if USE_SRGB
			pr *= pr, pg *= pg, pb *= pb;
			cr *= cr, cg *= cg, cb *= cb;
#endif

			pr *= cr;
			pg *= cg;
			pb *= cb;

#if USE_SRGB
			pr = sqrtf(pr);
			pg = sqrtf(pg);
			pb = sqrtf(pb);
#endif
#if 0
			float t = ((*c >> 24) & 0xFF) / 255.0f;
			pr += (cr - pr) * t;
			pg += (cg - pg) * t;
			pb += (cb - pb) * t;
			pr *= t;
			pg *= t;
			pb *= t;
#endif
			*pixel = ((unsigned)(pr * 255 + 0.5f) << 16)
					|((unsigned)(pg * 255 + 0.5f) <<  8)
					|((unsigned)(pb * 255 + 0.5f) <<  0);
		}
	}
#endif

	draw_rect(&game->stat_image, 0, 0, game->stat_image.width, game->stat_image.height, 0x555555);
	
	int dim = (game->stat_image.width) / game->map.width;
	if (dim < 1)
		dim = 1;
	int offset_x = (game->stat_image.width - game->map.width * dim) / 2;
	int offset_y = 10;
	for (int y = 0; y < game->map.height; y++)
	{
		for (int x = 0; x < game->map.width; x++)
		{
			int min_x = offset_x + dim * x;
			int min_y = offset_y + dim * y;
			char c = game->map.contents[y][x];
			unsigned int color = 0;
			if (c == '1')
				color = 0xff;
			else if (c == 'P')
				color = 0xffff00;
			else if (c == 'X')
				color = 0x005511;
			else if (c == 'E')
				color = (game->collected_count == game->map.collectibles_count ? 0x00ff0000 : 0x00ff00ff);
			else if (c == 'C')
				color = 0x005f0f0f;
			draw_rect(&game->stat_image, min_x, min_y, min_x + dim, min_y + dim, color);
		}
	}
	clock_t curr = clock() - t;
	double target_seconds_per_frame = dt;
	while ((double)curr / CLOCKS_PER_SEC < target_seconds_per_frame)
		curr = clock() - t;

	mlx_put_image_to_window(game->mlx, game->window, game->draw_image.img, 0, 0);
	return (0);
}

int main(int argc, char **argv)
{
	t_game	game = {0};

	if (argc != 2)
	{
		printf("Error\nExpected 1 argument\n");
		return (1);
	}
	srand(time(0));
	//todo: trim the string? ".ber?"
	if (!parse_map(&game.map, argv[1]))
		return (1);
	int window_width = 540;
	int window_height = 360;
	game.mlx = mlx_init();
	game.window = mlx_new_window(game.mlx, window_width, window_height, "so_long");

	game.draw_image.width = window_width;
	game.draw_image.height = window_height;

	if (game.draw_image.width < game.draw_image.height)
		game.cell_dim = game.draw_image.width / 16;
	else
		game.cell_dim = game.draw_image.height / 16;
	//game.cell_dim = 16;
	game.draw_image.img = mlx_new_image(game.mlx, window_width, window_height);
	game.draw_image.pixels = mlx_get_data_addr(game.draw_image.img, 
		&game.draw_image.bits_per_pixel, 
		&game.draw_image.line_length, 
		&game.draw_image.endian);

	game.stat_image.width = window_width - game.draw_image.width;
	game.stat_image.height = window_height;
	game.stat_image.bits_per_pixel = game.draw_image.bits_per_pixel;
	game.stat_image.line_length = game.draw_image.line_length;
	game.stat_image.pixels = game.draw_image.pixels + game.draw_image.width * (game.draw_image.bits_per_pixel / 8);

	game.light_image.width = game.draw_image.width;
	game.light_image.height = game.draw_image.height;
	game.light_image.line_length = game.light_image.width * 4;
	game.light_image.bits_per_pixel = 32;
	game.light_image.pixels = calloc(game.light_image.line_length * game.light_image.height, 1);

	assert(game.window && game.draw_image.img);

	for (int y = 0; y < game.map.height; y++)
	{
		for (int x = 0; x < game.map.width; x++)
		{
			if (game.map.contents[y][x] == 'P')
			{
				game.player_visual_y = game.player_y = y;
				game.player_visual_x = game.player_x = x;
			}	
			if (game.map.contents[y][x] == 'X')
			{
				game.enemy_visual_y = game.enemy_y = y;
				game.enemy_visual_x = game.enemy_x = x;
			}
		}
	}
	t_image img;
	img.img = mlx_png_file_to_image(game.mlx, "guy.png", &img.width, &img.height);
	img.pixels = mlx_get_data_addr(img.img,
		&img.bits_per_pixel, 
		&img.line_length, 
		&img.endian);
	game.player_image = img;
	printf("%d\n", img.bits_per_pixel);
	mlx_do_key_autorepeatoff(game.mlx);
	// read png in another program and output a simple format (with alpha)
#if 0
	game.player_image.img = mlx_xpm_file_to_image(game.mlx, "Idle.xpm", &game.player_image.width, &game.player_image.height);
	game.player_image.pixels = mlx_get_data_addr(game.player_image.img, 
	 	&game.player_image.bits_per_pixel, 
	 	&game.player_image.line_length, 
	 	&game.player_image.endian);
#endif
	
	assert(game.player_image.pixels && game.draw_image.img);
	mlx_hook(game.window, 2, 0, on_key_down, &game);
	mlx_hook(game.window, 3, 0, on_key_up, &game);
	mlx_hook(game.window, 17, 0, exit_game, &game);
	mlx_loop_hook(game.mlx, loop_hook, &game);
	mlx_loop(game.mlx);
}
