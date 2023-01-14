#include "so_long.h"
# define STBI_ONLY_PNG
# define STB_IMAGE_IMPLEMENTATION
# include "stb_image.h"

float last_frame_time = 0;
float dt = 1.0 / 30;

void reset_game(t_game *game);

int exit_game(t_game *game)
{
	mlx_destroy_image(game->mlx, game->window_image.img);
	mlx_destroy_window(game->mlx, game->window);
	exit(0);
	return (0);
}

float dist_sq(float x0, float y0, float x1, float y1)
{
	return (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
}

t_image load_image(char *filename)
{
    int w, h, n;
    unsigned char *pixels = stbi_load(filename, &w, &h, &n, 4);
    if (!pixels)
    {
        printf("failed to load %s\n", filename);
        assert(0);
    }
	if (n == 3)
		printf("Warning n == 3 for image: %s\n", filename);
    assert(n == 4 || n == 3);
    t_image img;
    img.width = w;
    img.height = h;
	img.line_length = w * 4;
	img.bits_per_pixel = 32;
    img.pixels = (char *)pixels;//malloc(w * h * 4);
    //pixels are 0xRRGGBBAA
	//todo: n == 3?
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
			unsigned int *pixel = (unsigned int *)(img.pixels + y * img.line_length +
					x * (img.bits_per_pixel / 8));
            uint32_t p = *pixel;
            uint32_t r = (p >> 0)  & 0xFF;
            uint32_t g = (p >> 8)  & 0xFF;
            uint32_t b = (p >> 16) & 0xFF;
            uint32_t a = (p >> 24) & 0xFF;
            *pixel = (a << 24) | (r << 16) | (g << 8) | (b << 0);
            pixel++;
        }
    }
    return img;
}

t_image flip_image_by_x(t_image *image)
{
	t_image res = *image;

	res.pixels = malloc(res.height * res.line_length);
	for (int y = 0; y < res.height; y++)
	{
		for (int x = 0; x < res.width; x++)
		{
			unsigned int *dest = (unsigned int *)(res.pixels + y * res.line_length +
					x * (res.bits_per_pixel / 8));
			unsigned int *src = (unsigned int *)(image->pixels + y * image->line_length +
					(image->width - x - 1) * (image->bits_per_pixel / 8));
			*dest = *src;
		}
	}
	return (res);
}

#if 0
enum {
	KEY_UP = 126,//13,
	KEY_DOWN = 125,//1,
	KEY_LEFT = 123,//0,
	KEY_RIGHT = 124,//2,
};
#else
enum {
	KEY_UP = 13,
	KEY_DOWN = 1,
	KEY_LEFT = 0,
	KEY_RIGHT = 2,
};
#endif

int last_keycode = -1;

int on_key_down(int keycode, t_game *game)
{
	(void)game;
	last_keycode = keycode; // multiple key per frame?
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
	if (keycode == last_keycode)
		last_keycode = -1;
	return (0);
}

void add_light_circle(t_game *game, int cx, int cy, int r, unsigned int color)
{
#if 1
	cx *= game->light_scale;
	cy *= game->light_scale;
	r *= game->light_scale;
#endif
	t_image *image = &game->light_image;
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

	float cr = (color >> 16) & 0xFF, cg = (color >> 8) & 0xFF, cb = (color >> 0) & 0xFF;
	char *row = image->pixels + min_y * image->line_length + min_x * 4;
	for (int y = min_y; y < max_y; y++)
	{
		unsigned *pixel = (unsigned *)row;
		for (int x = min_x; x < max_x; x++)
		{
			int dist_sq = (x - cx) * (x - cx) + (y - cy) * (y - cy);
			if (dist_sq <= r * r)
			{
				float dist = sqrtf(dist_sq);
				float t = 1 - dist / r;
				if (t > 1)
					t = 1;
				unsigned int p = *pixel;
				float pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = (p >> 0) & 0xFF;
				pr = pr + (cr  - pr) * t;
				pg = pg + (cg  - pg) * t;
				pb = pb + (cb  - pb) * t;
				*pixel = ((unsigned)(t * 255 + 0.5f) << 24)
						|((unsigned)(pr + 0.5f)      << 16)
						|((unsigned)(pg + 0.5f)      <<  8)
						|((unsigned)(pb + 0.5f)		 <<  0);
			}
			pixel++;
		}
		row += image->line_length;
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
	
	int r = (color >> 16) & 0xFF, g = (color >> 8) & 0xFF, b = color & 0xFF;
	float a = ((color >> 24) & 0xFF) / 255.0f;

	y = min_y;
	while (y < max_y)
	{
		x = min_x;
		while (x < max_x)
		{
			pixel = (unsigned int *)(image->pixels + y * image->line_length +
				x * (image->bits_per_pixel / 8));

			unsigned dest = *pixel;
			int dr = (dest >> 16) & 0xFF, dg = (dest >> 8) & 0xFF, db = dest & 0xFF;

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
	int x_min = min_x;
	int y_min = min_y;
	float xdiv = 1.0f / (max_x - min_x);
	float ydiv = 1.0f / (max_y - min_y);

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x > draw_image->width) max_x = draw_image->width;
    if (max_y > draw_image->height) max_y = draw_image->height;

	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			float tx = (float)(x - x_min) * xdiv;
			float ty = (float)(y - y_min) * ydiv;
			int ix = (tx * image->width);
			int iy = (ty * image->height);
			unsigned src =  *((unsigned *)(image->pixels + iy * image->line_length +
			ix * (image->bits_per_pixel / 8)));

			unsigned  *dest = (unsigned *)(draw_image->pixels + y * draw_image->line_length +
				x * (draw_image->bits_per_pixel / 8));
			unsigned p = *dest;
			int src_r = (src >> 16) & 0xFF, dest_r = (p >> 16) & 0xFF;
			int src_g = (src >> 8) & 0xFF, dest_g = (p >> 8) & 0xFF;
			int src_b = (src >> 0) & 0xFF, dest_b = (p >> 0) & 0xFF;
			float t = ((src >> 24) & 0xFF) / 255.0f;
			int r = (1 - t) * dest_r + t * src_r;
			int g = (1 - t) * dest_g + t * src_g;
			int b = (1 - t) * dest_b + t * src_b;
			*dest = (r << 16) | (g << 8) | b;
		}
	}
}

void update_dir(t_game *game, float *visual_x, int dx, float *vel_x, int *game_x, int game_y, int is_x, float a, int is_player)
{
	a -= (*vel_x) * 20;
	float delta = 0.5f * dt * dt * a + dt * (*vel_x);
	*visual_x += delta;
	*vel_x += a * dt;
	//todo: play with all these values
	if (!dx && fabsf(delta) < 0.1) 
	{
		int target;
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
		int dir = (target - (*visual_x)) > 0 ? 1 : -1;

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
	if (is_x)//todo: check wider range for enemy-player collision
	{
		char c1 = game->map.contents[game_y][(int)*visual_x];
		char c2 = game->map.contents[game_y][(int)(*visual_x + 1)];
		if (!is_player &&
				((c1 == 'X' && (int)(*visual_x) != *game_x)
			|| (c2 == 'X' && (int)(*visual_x + 1) != *game_x)))
			*visual_x = *game_x, *vel_x = 0;
		if (c1 == '1' || c2 == '1' || (!is_player && (c1 == 'E' || c2 == 'E')) 
			|| (is_player && (c1 == 'E' || c2 == 'E') 
				&& game->collected_count < game->map.collectibles_count) || (!is_player && (c1 == 'C' || c2 == 'C')))
			*visual_x = *game_x, *vel_x = 0;
		game->map.contents[game_y][*game_x] = '0';
		*game_x = roundf(*visual_x);
		char c = game->map.contents[game_y][*game_x];
		if ((is_player && c == 'X') || (!is_player && c == 'P'))
			game->player_dead = 1;
		if (is_player && c == 'C')
			game->collected_count++;
		if (is_player && c == 'E')
			exit(0);
		if (is_player)
			game->map.contents[game_y][*game_x] = 'P';
		else
			game->map.contents[game_y][*game_x] = 'X';
	} 
	else
	{
		char c1 = game->map.contents[(int)*visual_x][game_y];
		char c2 = game->map.contents[(int)(*visual_x + 1)][game_y];
		if (!is_player && 
			((c1 == 'X' && (int)(*visual_x) != *game_x) 
			|| (c2 == 'X' && (int)(*visual_x + 1) != *game_x)))
			*visual_x = *game_x, *vel_x = 0;
		if (c1 == '1' || c2 == '1' || (!is_player && (c1 == 'E' || c2 == 'E')) 
			|| (is_player && (c1 == 'E' || c2 == 'E') 
				&& game->collected_count < game->map.collectibles_count) || (!is_player && (c1 == 'C' || c2 == 'C')))
			*visual_x = *game_x, *vel_x = 0;
		game->map.contents[*game_x][game_y] = '0';
		*game_x = roundf(*visual_x);
		char c = game->map.contents[*game_x][game_y];
		if ((is_player && c == 'X') || (!is_player && c == 'P'))
			game->player_dead = 1;
		if (is_player && c == 'C')
			game->collected_count++;
		if (is_player && c == 'E')
			exit(0);
		if (is_player)
			game->map.contents[*game_x][game_y] = 'P';
		else
			game->map.contents[*game_x][game_y] = 'X';
	}
}

typedef struct s_particule_emitter {
	float base_x;
	float base_y;
	int count;
	int normalize_dir;
	float max_lifetime;
	int use_dir;
	int dx;
	int dy;
	float r, g, b;
} t_particule_emitter;

void emit_particules(t_game *game, t_particule_emitter *e)
{
	int	i;

	i = 0;
	//player other color: p->r = 1, p->g = 0.64, p->b = 0;
	while (i < e->count)
	{
		if (game->particule_count >= array_length(game->particules))
			break;
		t_particule *p = &game->particules[game->particule_count++];
		if (e->use_dir)
		{
			if (e->dx)
			{
				p->x = e->base_x - e->dx * (rand() % game->cell_dim);
				p->y = e->base_y + (rand() % game->cell_dim - game->cell_dim / 2);
				p->dx = -e->dx;
				p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
			}
			else
			{
				p->x = e->base_x + (rand() % game->cell_dim - game->cell_dim / 2);
				p->y = e->base_y - e->dy * (rand() % game->cell_dim);
				p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
				p->dy = -e->dy;
			}
		}
		else
		{
			p->x = e->base_x + (rand() % (game->cell_dim * 2) - game->cell_dim);
			p->y = e->base_y + (rand() % (game->cell_dim * 2) - game->cell_dim);
			p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
			p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
		}

		if (e->normalize_dir)
		{
			float l = sqrtf(p->dx * p->dx + p->dy * p->dy);
			if (l > 0.1)
			{
				p->dx /= l;
				p->dy /= l;
			}
		}
		p->lifetime = ((float)rand() / RAND_MAX) * e->max_lifetime;
		p->lifetime_left = p->lifetime;
		if (game->cell_dim < 5) // check this
			p->size = 1;
		else
			p->size = rand() % (game->cell_dim / 5) + 1;
		p->r = e->r;
		p->g = e->g;
		p->b = e->b;
		i++;
	}
}

int game_loop(t_game *game)
{
	clock_t t = clock();

	memcpy(game->draw_image.pixels, game->back_ground.pixels, game->draw_image.line_length * game->draw_image.height);
	memset(game->light_image.pixels, 0, game->light_image.line_length * game->light_image.height);
	for (int x = 0; x < game->map.width; x++)
	{
		for (int y = 0; y < game->map.height; y++)
		{
			int min_x = game->offset_x + x * game->cell_dim;
			int min_y = game->offset_y + y * game->cell_dim;
			int max_x = min_x + game->cell_dim;
			int max_y = min_y + game->cell_dim;
			int center_x = min_x + 0.5f * game->cell_dim;
			int center_y = min_y + 0.5f * game->cell_dim;
			char c = game->map.contents[y][x];
			if (c == 'E')
			{
				if (game->collected_count < game->map.collectibles_count)
					draw_image(&game->draw_image, &game->door[0], min_x, min_y, max_x, max_y);	
				else
				{
					add_light_circle(game, center_x, center_y, game->cell_dim * 8, 0xffffffff);
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
			else if (c == 'C')
			{
				emit_particules(game, &(t_particule_emitter){
							.base_x = (x + 0.5f) * game->cell_dim,
							.base_y = (y + 0.5f) * game->cell_dim,
							.max_lifetime = 0.5f,
							.r = 1, .g = 1, .b = 0,
							.normalize_dir = 1,
							.count = 1,
						});
				
				add_light_circle(game, center_x, center_y, game->cell_dim * 4, 0xffffff11);
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
				add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, 5 * game->cell_dim, 0xffffa501);
				draw_image(&game->draw_image, &game->torch[(game->torch_frame / 4) % 4], min_x, min_y, max_x, max_y);
			}
		}
	}
	game->torch_frame++;
	game->coin_frame++;

	if (!game->player_dead)
	{
		game->player_running = 0;
		if (last_keycode != -1)
		{
			int keycode = last_keycode; 
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
			last_keycode = -1;
		}
		update_dir(game, &game->player_visual_x, game->player_dx, &game->vel_x, &game->player_x, game->player_y, 1, game->player_dx * 200, 1);
		update_dir(game, &game->player_visual_y, game->player_dy, &game->vel_y, &game->player_y, game->player_x, 0, game->player_dy * 200, 1);
		if (game->player_dx || game->player_dy)
		{
			emit_particules(game, &(t_particule_emitter){
					.base_x = (game->player_visual_x + 0.5f) * game->cell_dim,
					.base_y = (game->player_visual_y + 0.5f) * game->cell_dim,
					.use_dir = 1,
					.dx = game->player_dx,
					.dy = game->player_dy,
					.max_lifetime = 1.f,
					.r = 0, .g = 1, .b = 1,
					.count = 3,
			});
		}
		t_image *img;
		float min_x = game->offset_x + game->player_visual_x * game->cell_dim;
		float min_y = game->offset_y + game->player_visual_y * game->cell_dim;
		if (game->player_running)
			img = &game->player_run[(game->player_frame / 4) % 4][game->player_dir];
		else
			img = &game->player_idle[(game->player_frame / 4) % 4][game->player_dir];
		game->player_frame++;
		add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 7, 0xffffffff);
		draw_image(&game->draw_image, img, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim);
	}
	
	for (int i = 0; i < game->enemies_count; i++)
	{
		t_enemy *e = game->enemies + i;

		t_image *img = 0;
		if (game->player_dead)
			e->mad = 0;

		e->mad = dist_sq(e->visual_x, e->visual_y, game->player_visual_x, game->player_visual_y) < 5 * 5;
		if (e->mad)
		{
			e->dx = 0;
			if (game->player_x > e->x)
				e->dx = 1;
			else if (game->player_x < e->x)
				e->dx = -1;

			e->dy = 0;
			if (game->player_y > e->y)
				e->dy = 1;
			else if (game->player_y < e->y)
				e->dy = -1;

			if (e->dx || fabsf(e->visual_x - e->x) > 0.05f)
				e->dy = 0;
			img = &game->enemy_run[(e->frame / 4) % 4][e->dx < 0];
			emit_particules(game, &(t_particule_emitter){
					.base_x = (e->visual_x + 0.5f) * game->cell_dim,
					.base_y = (e->visual_y + 0.5f) * game->cell_dim,
					.use_dir = 1,
					.dx = e->dx,
					.dy = e->dy,
					.max_lifetime = 1.f,
					.r = 1, .g = 0, .b = 0,
					.count = 2,
			});
		}
		else
		{
			e->dx = 0;
			e->dy = 0;
			img = &game->enemy_idle[(e->frame / 6) % 4][e->dx < 0];
		}
		update_dir(game, &e->visual_x, e->dx, &e->vel_x, &e->x, e->y, 1, e->dx * 100, 0);
		update_dir(game, &e->visual_y, e->dy, &e->vel_y, &e->y, e->x, 0, e->dy * 100, 0);
		e->frame++;

		float min_x = game->offset_x + e->visual_x * game->cell_dim;
		float min_y = game->offset_y + e->visual_y * game->cell_dim;

		float target = (e->mad ? 1 : 0);
		e->t += (target < e->t ? -1 : 1) * dt;
		if (e->t > 1)
			e->t = 1;
		if (e->t < 0)
			e->t = 0;
		float r0 = 1, g0 = 0.8, b0 = 0.7;
		float r1 = 1, g1 = 0.2, b1 = 0.2;
		float r = r0 * (1 - e->t) + r1 * e->t;
		float g = g0 * (1 - e->t) + g1 * e->t;
		float b = b0 * (1 - e->t) + b1 * e->t;
		unsigned c = ((unsigned)(r * 255.0f + 0.5f) << 16)
					|((unsigned)(g * 255.0f + 0.5f) <<  8)
					|((unsigned)(b * 255.0f + 0.5f) <<  0);
		add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 7, c);
		draw_image(&game->draw_image, img, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim);
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
			float min_x = game->offset_x + p->x - size * 0.5f;
			float min_y = game->offset_y + p->y - size * 0.5f;
			unsigned int color = ((unsigned)(t * 255 + 0.5f) << 24)
								|((unsigned)(p->r * 255 + 0.5f) << 16)
								|((unsigned)(p->g * 255 + 0.5f) <<  8)
								|((unsigned)(p->b * 255 + 0.5f) <<  0);
			draw_rect(&game->draw_image, min_x, min_y, min_x + size, min_y + size, color);
			i++;
		}
		p->lifetime_left -= dt;
	}

	float one_over_255 = 1.0f / 255;
#if 1
	char *row = game->draw_image.pixels;
	char *window_row1 = game->window_image.pixels;
	char *window_row2 = game->window_image.pixels + game->window_image.line_length;
	for (int y = 0; y < game->draw_image.height; y++)
	{
		unsigned *pixel = (unsigned *)row;
		unsigned *dest1 = (unsigned *)window_row1;
		unsigned *dest2 = (unsigned *)window_row2;
		unsigned *light = (unsigned *)(game->light_image.pixels + (y >> 2) * game->light_image.line_length);
		for (int x = 0; x < game->draw_image.width; x++)
		{
			unsigned c = *(light + (x >> 2)), p = *pixel;
			int cr = (c >> 16) & 0xFF, cg = (c >> 8) & 0xFF, cb = c & 0xFF;
			int pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = p & 0xFF;
#if 1
			pr *= cr * one_over_255;
			pg *= cg * one_over_255;
			pb *= cb * one_over_255;
#endif
			unsigned color = (pr << 16) | (pg << 8) | pb;
			
			*dest1++ = color;
			*dest1++ = color;
			*dest2++ = color;
			*dest2++ = color;
			pixel++;
		}
		window_row1 += game->window_image.line_length * 2;
		window_row2 += game->window_image.line_length * 2;
		row += game->draw_image.line_length;
	}
#else
	char *row = game->draw_image.pixels;
	for (int y = 0; y < game->draw_image.height; y++)
	{
		unsigned *pixel = (unsigned *)row;
		unsigned *light = (unsigned *)(game->light_image.pixels + (y >> 2) * game->light_image.line_length);
		for (int x = 0; x < game->draw_image.width; x++)
		{
			unsigned c = *(light + (x >> 2)), p = *pixel;
			int cr = (c >> 16) & 0xFF, cg = (c >> 8) & 0xFF, cb = c & 0xFF;
			int pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = p & 0xFF;
#if 1
			pr *= cr * one_over_255;
			pg *= cg * one_over_255;
			pb *= cb * one_over_255;
#endif
			unsigned color = (pr << 16) | (pg << 8) | pb;
			*pixel++ = color;
		}
		row += game->draw_image.line_length;
	}
	row = game->window_image.pixels;
	for (int y = 0; y < game->window_image.height; y++)
	{
		unsigned *pixel = (unsigned *)row;
		for (int x = 0; x < game->window_image.width; x++)
		{
			unsigned *src = (unsigned *)(game->draw_image.pixels 
					+ (y >> 1) * game->draw_image.line_length + (x >> 1) * 4);
			*pixel++ = *src;
		}
		row += game->window_image.line_length;
	}
#endif
	if (game->player_dead)
	{
		game->dead_t += dt * 0.8;
		assert(game->death_image.width == game->window_image.width);
		assert(game->death_image.height == game->window_image.height);
		float t = (game->dead_t > 1) ? 1 : game->dead_t;
		char *dest_row = game->window_image.pixels;
		char *src_row = game->death_image.pixels;
		for (int y = 0; y < game->window_image.height; y++)
		{
			unsigned *dest = (unsigned *)dest_row;
			unsigned *src = (unsigned *)src_row;
			for (int x = 0; x < game->window_image.width; x++)
			{
				unsigned s = *src;
				unsigned p = *dest;
				int src_r = (s >> 16) & 0xFF, dest_r = (p >> 16) & 0xFF;
				int src_g = (s >> 8) & 0xFF, dest_g = (p >> 8) & 0xFF;
				int src_b = (s >> 0) & 0xFF, dest_b = (p >> 0) & 0xFF;
				int r = (1 - t) * dest_r + t * src_r;
				int g = (1 - t) * dest_g + t * src_g;
				int b = (1 - t) * dest_b + t * src_b;
				*dest++ = (r << 16) | (g << 8) | b;
				src++;
			}
			dest_row += game->window_image.line_length;
			src_row += game->death_image.line_length;
		}
	}


	clock_t curr = clock() - t;
	double target_seconds_per_frame = dt;

	last_frame_time = ((double)curr / CLOCKS_PER_SEC) * 1000;
	//printf("%.2f\n", last_frame_time);

	while ((double)curr / CLOCKS_PER_SEC < target_seconds_per_frame)
		curr = clock() - t;

	mlx_put_image_to_window(game->mlx, game->window, game->window_image.img, 0, 0);

	char s[256];
	sprintf(s, "moves: %d", game->moves_count);
	mlx_string_put(game->mlx, game->window, game->window_image.width - strlen(s) * 10, 0, 0xffffff, s);
	sprintf(s, "time: %.2f", last_frame_time);
	mlx_string_put(game->mlx, game->window, game->window_image.width - strlen(s) * 10, 20, 0xffffff, s);

	if (game->dead_t > 1.25)
	{
		reset_game(game);
	}
	return (0);
}

typedef struct zf_header {
	int width;
	int height;
	int count;
}zf_header;

void reset_game(t_game *game)
{
	if (game->map.contents)
	{
		for (int i = 0; i < game->map.height; i++)
			free(game->map.contents[i]);
		free(game->map.contents);
		free(game->enemies);
	}
	game->particule_count = 0;
		
	game->moves_count = 0;
	game->collected_count = 0;
	game->player_frame = 0;
	game->player_running = 0;
	game->player_dx = game->player_dx = game->vel_x = game->vel_y = 0;
	game->coin_frame = 0;
	game->dead_t = 0;
	game->player_dead = 0;
	game->torch_frame = 0;
	game->map = game->original_map;

	game->map.contents = malloc(game->map.height * sizeof(char *));
	for (int i = 0; i < game->map.height; i++)
		game->map.contents[i] = strdup(game->original_map.contents[i]);
	game->enemies_count = 0;
	for (int y = 0; y < game->map.height; y++)
	{
		for (int x = 0; x < game->map.width; x++)
		{
			if (game->map.contents[y][x] == 'X')
				game->enemies_count++;
		}
	}
	game->enemies = calloc(game->enemies_count, sizeof(t_enemy));
	int e = 0;
	for (int y = 0; y < game->map.height; y++)
	{
		for (int x = 0; x < game->map.width; x++)
		{
			if (game->map.contents[y][x] == 'P')
			{
				game->player_visual_y = game->player_y = y;
				game->player_visual_x = game->player_x = x;
			}	
			if (game->map.contents[y][x] == 'X')
			{
				game->enemies[e].visual_y = game->enemies[e].y = y;
				game->enemies[e].visual_x = game->enemies[e].x = x;
				e++;
			}
		}
	}
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
	if (!parse_map(&game.original_map, argv[1]))
		return (1);
	reset_game(&game);
	int window_width = 1920;
	int window_height = 1080;
	game.mlx = mlx_init();
	game.window = mlx_new_window(game.mlx, window_width, window_height, "so_long");

	//todo: render the background in full scale?
	
	game.window_scale = 2;
	game.draw_image.width = window_width  / game.window_scale;
	game.draw_image.height = window_height / game.window_scale;
	game.draw_image.line_length = game.draw_image.width * 4;
	game.draw_image.pixels = calloc(game.draw_image.line_length * game.draw_image.height, 1);
	game.draw_image.bits_per_pixel = 32;

	int c1 = (game.draw_image.width) / game.map.width;
	int c2 = (game.draw_image.height) / game.map.height;

	game.cell_dim = c1;
	if (c2 < c1)
		game.cell_dim = c2;
	//if (game.cell_dim % 2)//test this
	//	game.cell_dim--;
	game.offset_x = (game.draw_image.width - game.cell_dim * game.map.width) / 2;
	game.offset_y = (game.draw_image.height - game.cell_dim * game.map.height) / 2;
	printf("offset_x:%d offset_y:%d\n", game.offset_x, game.offset_y);

	printf("cell_dim:%d map_width:%d map_height:%d\n", game.cell_dim, game.map.width, game.map.height);
	game.window_image.width = window_width;
	game.window_image.height = window_height;
	game.window_image.img = mlx_new_image(game.mlx, window_width, window_height);
	game.window_image.pixels = mlx_get_data_addr(game.window_image.img, 
		&game.window_image.bits_per_pixel, 
		&game.window_image.line_length, 
		&game.window_image.endian);
	memset(game.window_image.pixels, 0, game.window_image.line_length * game.window_image.height);

	game.light_scale = 0.25f; // can we optimize out to .25f?
	game.light_image.width = game.draw_image.width * game.light_scale;
	game.light_image.height = game.draw_image.height * game.light_scale;
	game.light_image.line_length = game.light_image.width * 4;
	game.light_image.bits_per_pixel = 32;
	game.light_image.pixels = calloc(game.light_image.line_length * game.light_image.height, 1);


	game.floors = malloc(game.map.width * game.map.height * sizeof(int));
	for (int y = 0; y < game.map.height; y++)
	{
		for (int x = 0; x < game.map.width; x++)
		{
			int p = rand() % 4;
			if (!(rand() % 5))
			{
				p = rand() % 3 + 4;
			}
			game.floors[y * game.map.width + x] = p;
		}
	}

	char s[256];
	for (int i = 0; i < 4; i++)
	{
		sprintf(s, "knight_m_idle_anim_f%d.png", i);
		game.player_idle[i][0] = load_image(s);
		sprintf(s, "knight_m_run_anim_f%d.png", i);
		game.player_run[i][0] = load_image(s);
#if 1
		game.player_run[i][0].height -= 7;
		game.player_run[i][0].pixels += 7 * game.player_idle[i][0].line_length;
		game.player_idle[i][0].height -= 7;
		game.player_idle[i][0].pixels += 7 * game.player_idle[i][0].line_length; 

#endif
		game.player_idle[i][1] = flip_image_by_x(&game.player_idle[i][0]);
		game.player_run[i][1] = flip_image_by_x(&game.player_run[i][0]);

		sprintf(s, "big_demon_idle_anim_f%d.png", i);
		game.enemy_idle[i][0] = load_image(s);
		sprintf(s, "big_demon_run_anim_f%d.png", i);
		game.enemy_run[i][0] = load_image(s);

		game.enemy_run[i][0].height -= 3;
		game.enemy_run[i][0].pixels += 3 * game.enemy_idle[i][0].line_length;
		game.enemy_idle[i][0].height -= 3;
		game.enemy_idle[i][0].pixels += 3 * game.enemy_idle[i][0].line_length; 

		game.enemy_idle[i][1] = flip_image_by_x(&game.enemy_idle[i][0]);
		game.enemy_run[i][1] = flip_image_by_x(&game.enemy_run[i][0]);

		sprintf(s, "coin_anim_f%d.png", i);
		game.coin[i] = load_image(s);
		
	}
	for (int i = 0; i < 8; i++)
	{
		sprintf(s, "floor_%d.png", i + 1);
		game.floor[i] = load_image(s);
		if (i < 4)
		{
			sprintf(s, "torch_%d.png", i + 1);
			game.torch[i] = load_image(s);
		}
	}
	game.hole = load_image("hole.png");
	game.door[0] = load_image("doors_leaf_closed.png");
	game.door[1] = load_image("doors_leaf_open.png");
	game.wall_top_left = load_image("wall_top_left.png");
	game.wall_top_right = load_image("wall_top_right.png");
	game.wall_bottom_left = load_image("wall_bottom_left.png");
	game.wall_bottom_right = load_image("wall_bottom_right.png");
	game.wall_top = load_image("wall_top.png");
	game.wall_bottom = load_image("wall_bottom.png");
	game.wall_left = load_image("wall_left.png");
	game.wall_right = load_image("wall_right.png");
	game.wall_corner = game.wall_bottom;
	game.wall_corner.width = 4;
	game.floor_ladder = load_image("floor_ladder.png");
	game.death_image = load_image("death.png");


	// must be the same as draw_image
	game.back_ground.width = game.draw_image.width;
	game.back_ground.height = game.draw_image.height;
	game.back_ground.line_length = game.back_ground.width * 4;
	game.back_ground.bits_per_pixel = 32;
	game.back_ground.pixels = calloc(1, game.back_ground.line_length * game.back_ground.height);
	

	for (int x = 0; x < game.map.width; x++)
	{
		for (int y = 0; y < game.map.height; y++)
		{
			// window_center_x - camera_x * game.cell_dim
			char c = game.map.contents[y][x];
			int min_x = game.offset_x + x * game.cell_dim;
			int min_y = game.offset_y + y * game.cell_dim;
			int max_x = min_x + game.cell_dim;
			int max_y = min_y + game.cell_dim;
			int p = game.floors[y * game.map.width + x];
			//todo: render something under the door (checkout small map)
			if (c == '1')
			{
				t_image *img = 0;

#if 1
				if (!x && !y) // top-left
				{
				}
				else if (x == game.map.width - 1 && !y) // top - right
				{

				}
				else if (!x && y == game.map.height - 1) // bottom - left
				{

				}
				else if (x == game.map.width - 1 && y == game.map.height - 1) // bottom-right
				{

				}
#endif
				else if (!x) // left
				{
					img = &game.wall_left;
				}
				else if (x == game.map.width - 1) // right
				{
					img = &game.wall_right;
				}
				else if (!y) // top
				{
					img = &game.wall_top;
				}
				else if (y == game.map.height - 1) // bottom
				{
					img = &game.wall_bottom;
					min_y -= game.cell_dim * 0.75;
					max_y -= game.cell_dim * 0.75;
				}
				else
					img = &game.hole;
				if (img)
					draw_image(&game.back_ground, img, min_x, min_y, max_x, max_y);
			}
			else if (c == 'E')
				draw_image(&game.back_ground, &game.floor_ladder, min_x, min_y, max_x, max_y);
			else
				draw_image(&game.back_ground, &game.floor[p], min_x, min_y, max_x, max_y);
			min_x = game.offset_x + x * game.cell_dim;
			min_y = game.offset_y + y * game.cell_dim;
			max_x = min_x + game.cell_dim;
			max_y = min_y + game.cell_dim;
			//draw_rect_outline(&game.back_ground, min_x, min_y, max_x, max_y, 1, 0xffffffff);
		}
	}
	game.place_torch = calloc(game.map.width * game.map.height, sizeof(int));
	game.place_torch[0] = 1;
	game.place_torch[game.map.width - 2] = 1;
	game.place_torch[(game.map.height - 2) * game.map.width] = 1;
	game.place_torch[(game.map.height - 2) * game.map.width + game.map.width - 2] = 1;
	for (int y = 0; y < game.map.height - 1; y++)
	{
		for (int x = 0; x < game.map.width - 1; x++)
		{

			int can = (x == 0 || x == game.map.width - 2 || y == 0 || y == game.map.height - 2);
			for (int i = y - 4; i < y + 4; i++)
			{
				for (int j = x - 4; j < x + 4; j++)
				{
					if (i >= 0 && j >= 0 && i < game.map.height && j < game.map.width &&
							game.place_torch[i * game.map.width + j])
						can = 0;
				}
			}
			if (can && rand() % 10 == 0)
			{
				game.place_torch[y * game.map.width + x] = 1;
			}
		}
	}
	mlx_do_key_autorepeatoff(game.mlx);
	mlx_hook(game.window, 2, 0, on_key_down, &game);
	mlx_hook(game.window, 3, 0, on_key_up, &game);
	mlx_hook(game.window, 17, 0, exit_game, &game);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
}
