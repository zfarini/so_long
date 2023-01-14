#include "so_long.h"


float last_frame_time = 0;
char *images[64];

void restart_game(t_game *game);

int exit_game(t_game *game, int failed)
{
	int i;

	if (game->mlx)
	{
		mlx_destroy_image(game->mlx, game->window_image.img);
		mlx_destroy_window(game->mlx, game->window);
	}
	//why free here idk?
	i = 0;
	while (i < game->original_map.height)
	{
		if (game->original_map.contents)
			free(game->original_map.contents[i]);
		if (game->map.contents)
			free(game->map.contents[i]);
		i++;
	}
	free(game->original_map.contents);
	free(game->map.contents);
	free(game->back_ground.pixels);
	free(game->draw_image.pixels);
	i = 0;
	while (i < array_length(game->images))
		free(images[i++]);
	exit(failed);
	return (0);
}

float dist_sq(float x0, float y0, float x1, float y1)
{
	return (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
}

typedef struct s_color_rgb {
	int	r;
	int	g;
	int	b;
}	t_color_rgb;

unsigned int	lerp_color(unsigned color1, unsigned color2, float t)
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


float	clamp(float x, float min, float max)
{
	if (x < min)
		x = min;
	if (x > max)
		x = max;
	return (x);
}

int last_keycode = -1;

int on_key_down(int keycode, t_game *game)
{
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

void add_light_circle(t_game *game, int cx, int cy, int r, unsigned int color)
{
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	float cr, cg, cb;
	int y, x;
	unsigned *pixel;
	unsigned p;
	float pr, pg, pb;
	int dist_sq;
	float t;
	char *row;

	r *= game->cell_dim;
#if 1
	cx *= game->light_scale;
	cy *= game->light_scale;
	r *= game->light_scale;
#endif
	t_image *image = &game->light_image;
	min_x = cx - r;
	min_y = cy - r;
	max_x = cx + r;
	max_y = cy + r;
	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	if (max_x > image->width)
		max_x = image->width;
	if (max_y > image->height)
		max_y = image->height;

	cr = (color >> 16) & 0xFF, cg = (color >> 8) & 0xFF, cb = (color >> 0) & 0xFF;
	row = image->pixels + min_y * image->line_length + min_x * 4;
	y = min_y;
	while (y < max_y)
	{
		pixel = (unsigned *)row;
		x = min_x;
		while (x < max_x)
		{
			dist_sq = (x - cx) * (x - cx) + (y - cy) * (y - cy);
			if (dist_sq <= r * r)
			{
				t = 1 - sqrtf(dist_sq) / r;
				if (t > 1)
					t = 1;
				p = *pixel;
				pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = (p >> 0) & 0xFF;
				pr = pr + (cr  - pr) * t;
				pg = pg + (cg  - pg) * t;
				pb = pb + (cb  - pb) * t;
				*pixel = ((unsigned)(t * 255 + 0.5f) << 24)
						|((unsigned)(pr + 0.5f)      << 16)
						|((unsigned)(pg + 0.5f)      <<  8)
						|((unsigned)(pb + 0.5f)		 <<  0);
			}
			pixel++;
			x++;
		}
		row += image->line_length;
		y++;
	}
}

void draw_rect(t_image *image, int min_x, int min_y, int max_x, int max_y, unsigned int color)
{
	int		y;
	int		x;
	unsigned int	*pixel;
	int r, g, b;
	float a;
	unsigned dest;
	int dr, dg, db;

	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	if (max_x > image->width)
		max_x = image->width;
	if (max_y > image->height)
		max_y = image->height;
	
	r = (color >> 16) & 0xFF, g = (color >> 8) & 0xFF, b = color & 0xFF;
	a = ((color >> 24) & 0xFF) / 255.0f;
	y = min_y;
	while (y < max_y)
	{
		x = min_x;
		while (x < max_x)
		{
			pixel = (unsigned int *)(image->pixels + y * image->line_length +
				x * 4);

			dest = *pixel;
			dr = (dest >> 16) & 0xFF, dg = (dest >> 8) & 0xFF, db = dest & 0xFF;

			dr = (1 - a) * dr + a * r;
			dg = (1 - a) * dg + a * g;
			db = (1 - a) * db + a * b;
			*pixel = (dr << 16) | (dg << 8) | db;
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

void draw_image(t_image *draw_image, t_image *image, int min_x, int min_y, int max_x, int max_y)
{
	int x_min;
	int y_min;
	float xdiv;
	float ydiv;
	float tx;
	float ty;
	int ix;
	int iy;
	unsigned src;
	unsigned *dest;
	unsigned p;
	int src_r, dest_r, src_g, dest_g, src_b, dest_b;
	float t;
	int r, g, b;
	int y, x;

	x_min = min_x;
	y_min = min_y;
	xdiv = 1.0f / (max_x - min_x);
	ydiv = 1.0f / (max_y - min_y);
	if (min_x < 0) min_x = 0;
	if (min_y < 0) min_y = 0;
	if (max_x > draw_image->width) max_x = draw_image->width;
	if (max_y > draw_image->height) max_y = draw_image->height;

	y = min_y;
	while (y < max_y)
	{
		x = min_x;
		while (x < max_x)
		{
			tx = (float)(x - x_min) * xdiv;
			ty = (float)(y - y_min) * ydiv;
			ix = (tx * image->width);
			iy = (ty * image->height);
			src =  *((unsigned *)(image->pixels + iy * image->line_length +
			ix * 4));

			dest = (unsigned *)(draw_image->pixels + y * draw_image->line_length +
				x * 4);
			p = *dest;
			src_r = (src >> 16) & 0xFF, dest_r = (p >> 16) & 0xFF;
			src_g = (src >> 8) & 0xFF, dest_g = (p >> 8) & 0xFF;
			src_b = (src >> 0) & 0xFF, dest_b = (p >> 0) & 0xFF;
			t = ((src >> 24) & 0xFF) / 255.0f;
			r = (1 - t) * dest_r + t * src_r;
			g = (1 - t) * dest_g + t * src_g;
			b = (1 - t) * dest_b + t * src_b;
			*dest = (r << 16) | (g << 8) | b;
			x++;
		}
		y++;
	}
}

void update_dir(t_game *game, float *visual_x, int dx, float *vel_x, int *game_x, int game_y, int is_x, float a, int is_player)
{
	float delta;
	char c1;
	char c2;
	char *curr;
	int target;
	int dir;

	a -= (*vel_x) * 20;
	delta = 0.5f * dt * dt * a + dt * (*vel_x);
	*visual_x += delta;
	*vel_x += a * dt;
	//todo: play with all these values
	if (!dx && fabsf(delta) < 0.2f) 
	{
		if (delta > 0)
		{
			if (*visual_x - floorf(*visual_x) < 0.2f) 
				target = floorf(*visual_x);
			else
				target = ceilf(*visual_x);
		}
		else
		{
			if (ceilf(*visual_x) - *visual_x < 0.2f)
				target = ceilf(*visual_x);
			else
				target = floorf(*visual_x);
		}
		dir = (target - (*visual_x)) > 0 ? 1 : -1;

		*visual_x += dir * dt * 2;
		if ((dir > 0 && *visual_x >= target)
		|| (dir < 0 && *visual_x <= target))
		{
			*vel_x = 0;
			*visual_x = target;
		}
	}
	else if (is_player)
	{
		game->player_running = 1;
	}
	if (is_player && roundf(*visual_x) != *game_x)
		game->moves_count++;

	if (is_x)
	{
		c1 = game->map.contents[game_y][(int)*visual_x];
		c2 = game->map.contents[game_y][(int)(*visual_x + 1)];
		curr = &game->map.contents[game_y][*game_x];
	}
	else
	{
		c1 = game->map.contents[(int)*visual_x][game_y];
		c2 = game->map.contents[(int)(*visual_x + 1)][game_y];
		curr = &game->map.contents[*game_x][game_y];
	}
	
	//check collision
	if (!is_player &&
		((c1 == 'X' && (int)(*visual_x) != *game_x)
		|| (c2 == 'X' && (int)(*visual_x + 1) != *game_x)))
		*visual_x = *game_x, *vel_x = 0;
	if (c1 == '1' || c2 == '1' || (!is_player && (c1 == 'E' || c2 == 'E')) 
		|| (is_player && (c1 == 'E' || c2 == 'E') 
			&& game->collected_count < game->map.collectibles_count) || (!is_player && (c1 == 'C' || c2 == 'C')))
		*visual_x = *game_x, *vel_x = 0;

	*game_x = roundf(*visual_x);
	*curr = '0';
	if (is_x)
		curr = &game->map.contents[game_y][*game_x];
	else
		curr = &game->map.contents[*game_x][game_y];
	if ((is_player && *curr == 'X') || (!is_player && *curr == 'P'))
		game->player_dead = 1;
	if (is_player && *curr == 'C')
		game->collected_count++;
	if (is_player && *curr == 'E')
		exit_game(game, 0);
	if (is_player)
		*curr = 'P';
	else
		*curr = 'X';
}



void draw_map(t_game *game)
{
	int x;
	int y;
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	int center_x;
	int center_y;

	memcpy(game->draw_image.pixels, game->back_ground.pixels, game->draw_image.line_length * game->draw_image.height);
	
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			min_x = game->offset_x + x * game->cell_dim;
			min_y = game->offset_y + y * game->cell_dim;
			max_x = min_x + game->cell_dim;
			max_y = min_y + game->cell_dim;
			center_x = min_x + 0.5f * game->cell_dim;
			center_y = min_y + 0.5f * game->cell_dim;

			if (game->map.contents[y][x] == 'E')
			{
				if (game->collected_count < game->map.collectibles_count)
					draw_image(&game->draw_image, &game->door[0], min_x, min_y, max_x, max_y);	
				else
				{
					add_light_circle(game, center_x, center_y, 8, 0xffffffff);
					draw_image(&game->draw_image, &game->door[1], min_x, min_y, max_x, max_y);	
					emit_particules(game, &(t_particule_emitter){
							.base_x = (x + 0.5f) * game->cell_dim,
							.base_y = (y + 0.5f) * game->cell_dim,
							.max_lifetime = 1.0f,
							.r = 1, .g = 1, .b = 1,
							.normalize_dir = 1,
							.count = 3,
					});
				}
			}
			else if (game->map.contents[y][x] == 'C')
			{
				emit_particules(game, &(t_particule_emitter){
							.base_x = (x + 0.5f) * game->cell_dim,
							.base_y = (y + 0.5f) * game->cell_dim,
							.max_lifetime = 0.35f,
							.r = 1, .g = 1, .b = 0,
							.normalize_dir = 1,
							.count = 1,
						});
				
				add_light_circle(game, center_x, center_y, 4, 0xffffff11);
				draw_image(&game->draw_image, &game->coin[(game->coin_frame / 4) % 4], min_x, min_y, max_x, max_y); 
			}
			if (game->place_torch[y * game->map.width + x])
			{
				min_x += game->cell_dim * 0.6f;
				min_y += game->cell_dim * 0.35f;
				if (y == game->map.height - 2 && (x == 0 || x == game->map.width - 2))
					min_y += game->cell_dim * .25f;
				if (x == game->map.width - 2 && (y == 0 || y == game->map.height - 2))
					min_x += game->cell_dim * 0.15f;
				if (x == 0 && (y == 0 || y == game->map.height - 2))
					min_x -= game->cell_dim * 0.1f;
				max_x = min_x + game->cell_dim * 0.75;
				max_y = min_y + game->cell_dim * 0.75;
				emit_particules(game, &(t_particule_emitter){
							.base_x = (x + 0.5f) * game->cell_dim,
							.base_y = (y + 0.5f) * game->cell_dim,
							.max_lifetime = .75f,
							.r = 1, .g = .64, .b = 0,
							.count = 1,
						});
				add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, 5, 0xffffa501);
				draw_image(&game->draw_image, &game->torch[(game->torch_frame / 4) % 4], min_x, min_y, max_x, max_y);
			}
			x++;
		}
		y++;
	}
	game->torch_frame++;
	game->coin_frame++;
}

typedef struct s_draw_info {
	int sr, sg, sb;
	int dr, dg, db;
} t_draw_info;

void add_light_and_draw_to_window_image(t_game *game)
{
	float one_over_255 = 1.0f / 255;
	char *row = game->draw_image.pixels;
	char *window_row1 = game->window_image.pixels;
	char *window_row2 = game->window_image.pixels + game->window_image.line_length;
	unsigned *pixel;
	unsigned *light;
	unsigned c, p;
	int cr, cg, cb;
	int pr, pg, pb;
	unsigned color;
	unsigned *dest1;
	unsigned *dest2;
	int y;
	int x;

	y = 0;
	while (y < game->draw_image.height)
	{
		pixel = (unsigned *)row;
		light = (unsigned *)(game->light_image.pixels + (y >> 2) * game->light_image.line_length);
		x = 0;
		while (x < game->draw_image.width)
		{
			c = *(light + (x >> 2)), p = *pixel;
			cr = (c >> 16) & 0xFF, cg = (c >> 8) & 0xFF, cb = c & 0xFF;
			pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = p & 0xFF;
			pr *= cr * one_over_255;
			pg *= cg * one_over_255;
			pb *= cb * one_over_255;
			color = (pr << 16) | (pg << 8) | pb;
			dest1 = (unsigned *)(game->window_image.pixels
					+ (y << 1) * game->window_image.line_length
					+ (x << 1) * (game->window_image.bits_per_pixel / 8));	
			dest2 = (unsigned *)((char *)dest1 + game->window_image.line_length);	
			*dest1 = color;
			*(dest1 + 1) = color;
			*dest2 = color;
			*(dest2 + 1) = color;
			pixel++;
			x++;
		}
		window_row1 += game->window_image.line_length * 2;
		window_row2 += game->window_image.line_length * 2;
		row += game->draw_image.line_length;
		y++;
	}
}

void draw_death_screen(t_game *game)
{
	float t;
	unsigned *dest;
	unsigned src;
	int y;
	int x;
	unsigned p;
	int src_r, src_g, src_b;
	int dest_r, dest_g, dest_b;
	int r, g, b;
	int ox, oy;

	ox = 700;
	oy = 250;
	if (!game->dead_t)
		game->death_count++;
	game->dead_t += dt * 0.8;
	t = (game->dead_t > 1) ? 1 : game->dead_t;
	y = 0;
	while (y < game->window_image.height)
	{
		x = 0;
		while (x < game->window_image.width)
		{
			dest = (unsigned *)(game->window_image.pixels
					+ y * game->window_image.line_length
					+ x * (game->window_image.bits_per_pixel / 8));
			src = 0;

			if (y >= oy && y < oy + game->death_image.height &&
				x >= ox && x < ox + game->death_image.width)
			{
				src = *((unsigned *)(game->death_image.pixels 
							+ (y - oy) * game->death_image.line_length 
							+ (x - ox) * 4));
			}
			p = *dest;
			src_r = (src >> 16) & 0xFF, dest_r = (p >> 16) & 0xFF;
			src_g = (src >> 8) & 0xFF, dest_g = (p >> 8) & 0xFF;
			src_b = (src >> 0) & 0xFF, dest_b = (p >> 0) & 0xFF;
			r = (1 - t) * dest_r + t * src_r;
			g = (1 - t) * dest_g + t * src_g;
			b = (1 - t) * dest_b + t * src_b;
			*dest = (r << 16) | (g << 8) | b;		
			x++;
		}
		y++;
	}
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

int main(int argc, char **argv)
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
	mlx_hook(game.window, 17, 0, exit_game, &game);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
	return (0);
}
