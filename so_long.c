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

for collectibles just pre render like 4 backgrouds for each animation?
and if it dies then replace it in the 4 backgrounds
*/
#include "so_long.h"
#include <math.h>
#include <time.h>
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define array_length(arr) ((int)(sizeof(arr) / sizeof(*arr)))

#define SCROLL 0
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
    assert(n == 4);
    t_image img;
    img.width = w;
    img.height = h;
	img.line_length = w * 4;
	img.bits_per_pixel = 32;
    img.pixels = (char *)pixels;//malloc(w * h * 4);
    //pixels are 0xRRGGBBAA
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
	(void)game;
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
	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			int dist_sq = (x - cx) * (x - cx) + (y - cy) * (y - cy);
			if (dist_sq <= r * r)
			{
				float dist = sqrtf(dist_sq);
				float t = 1 - dist / r;
				//t *= t;
				if (t > 1)
					t = 1;
				unsigned int *pixel = (unsigned int *)(image->pixels + y * image->line_length +
					x * (image->bits_per_pixel / 8));
				unsigned int p = *pixel;
				float pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = (p >> 0) & 0xFF;

				//float pt = ((*pixel >> 24) & 0xFF) / 255.0f;
				pr = pr + (cr * t  - pr) * t;
				pg = pg + (cg * t - pg) * t;
				pb = pb + (cb * t - pb) * t;
				*pixel = ((unsigned)(t * 255 + 0.5f) << 24)
						|((unsigned)(pr + 0.5f)      << 16)
						|((unsigned)(pg + 0.5f)      <<  8)
						|((unsigned)(pb + 0.5f)		 <<  0);
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


void draw_circle_outline(t_image *image, int cx, int cy, int r, int thickness, unsigned int color)
{
	int min_x = cx - r;
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
	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			int dist = (x - cx) * (x - cx) + (y - cy) * (y - cy);
			if (dist >= (r - thickness) * (r - thickness)  && dist <= r * r)
			{
				unsigned int *pixel = (unsigned *)(image->pixels + y * image->line_length + 
						x * (image->bits_per_pixel / 8));
				*pixel = color;
			}
		}
	}
}

int font_advance_x;
int font_line_height;
uint8_t *font_bitmaps[256];

void draw_char(t_image *image, int c, int min_x, int min_y, unsigned int color)
{
    if (min_y > image->height || min_x > image->width)
        return;
    if (c < 32 || c >= 127)
    {
        draw_rect(image, min_x, min_y, min_x + font_advance_x,
                  min_y + font_line_height, color);
        return;
    }
    int init_min_x = min_x;
    int init_min_y = min_y;
    int max_x = min_x + font_advance_x;
    int max_y = min_y + font_line_height;

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x > image->width) max_x = image->width;
    if (max_y > image->height) max_y = image->height;

	float r, g, b;
	color_u32_to_rgb01(color, &r, &g, &b);

    char *row = image->pixels + min_y * image->line_length + min_x * (image->bits_per_pixel / 8);
    uint8_t *src_row = &font_bitmaps[c][(min_y - init_min_y)  * font_advance_x + (min_x - init_min_x)];
    for (int y = min_y; y < max_y; y++)
    {
        uint32_t *pixel = (uint32_t *)row;
        uint8_t *src_pixel = src_row;
        for (int x = min_x; x < max_x; x++)
        {
            uint8_t grey = *src_pixel++;
            float a = grey / 255.0f;
            uint32_t p = *pixel;
            float dr = ((p >> 16) & 0xFF) / 255.0f;
            float dg = ((p >> 8) & 0xFF) / 255.0f;
            float db = ((p >> 0) & 0xFF) / 255.0f;
            dr = (1 - a) * dr + a * r;
            dg = (1 - a) * dg + a * g;
            db = (1 - a) * db + a * b;
            
            *pixel = ((uint32_t)(dr * 255 + 0.5f) << 16) |
                     ((uint32_t)(dg * 255 + 0.5f) <<  8) |
                     ((uint32_t)(db * 255 + 0.5f) <<  0);
            pixel++;
        }
        src_row += font_advance_x;
        row += image->line_length;
    }
}

int draw_text(t_image *image, char *s, int min_x, int min_y, unsigned int color)
{
    if (min_y > image->height || min_x > image->width)
        return 0;
    float x = min_x;
    float y = min_y;
    for (int i = 0; s[i]; i++)
    {
		if (s[i] == '\n')
		{
			y += font_line_height;
			x = min_x;
			continue;
		}
        draw_char(image, s[i], x, y, color);
        x += font_advance_x;
    }
    return (x - min_x);
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
			uint src =  *((uint *)(image->pixels + iy * image->line_length +
			ix * (image->bits_per_pixel / 8)));

			uint  *dest = (uint *)(draw_image->pixels + y * draw_image->line_length +
				x * (draw_image->bits_per_pixel / 8));
			uint p = *dest;
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

#include <string.h>

float dt = 1.0 / 60;

int check_map_p(t_game *game, int x, int y)
{
	char c = game->map.contents[y][x];
	if (c != '1' && c != 'E')
		return (1);
	return (0);
}

void update_dir(t_game *game, float *visual_x, int dx, float *vel_x, int *game_x, int game_y, int is_x, float a)
{
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
	else
		game->player_running = 1;
	if (is_x)
	{
		if (!check_map_p(game, *visual_x, game_y) || !check_map_p(game, *visual_x + 1, game_y))
			*visual_x = *game_x, *vel_x = 0;
		game->map.contents[game_y][*game_x] = '0';
		*game_x = roundf(*visual_x);
		if (game->map.contents[game_y][*game_x] == 'C')
			game->collected_count++;
		game->map.contents[game_y][*game_x] = 'P';
	}
	else
	{
		if (!check_map_p(game, game_y, *visual_x) || !check_map_p(game, game_y, *visual_x + 1))
			*visual_x = *game_x, *vel_x = 0;
		game->map.contents[*game_x][game_y] = '0';
		*game_x = roundf(*visual_x);
		if (game->map.contents[*game_x][game_y] == 'C')
			game->collected_count++;
		game->map.contents[*game_x][game_y] = 'P';
	}
}


int loop_hook(t_game *game)
{
	clock_t t = clock();

	game->player_running = 0;
	if (last_keycode != -1)
	{
		int keycode = last_keycode; // take the last inputed move?
		if (keycode == 13) //'W'
			game->player_dy = -1, game->player_dx = 0;
		else if (keycode == 0) // 'A'
		{
			game->player_dir = 1;
			game->player_dx = -1, game->player_dy = 0;
		}
		else if (keycode == 1) // 'S'
			game->player_dy = 1, game->player_dx = 0;
		else if (keycode == 2)
		{
			game->player_dir = 0;
			game->player_dx = 1, game->player_dy = 0;
		}
		last_keycode = -1;
	}
	//if (game->player_dx || game->player_dy)
	{
		update_dir(game, &game->player_visual_x, game->player_dx, &game->vel_x, &game->player_x, game->player_y, 1, game->player_dx * 400);
		update_dir(game, &game->player_visual_y, game->player_dy, &game->vel_y, &game->player_y, game->player_x, 0, game->player_dy * 400);
	}
	for (int i = 0; i < game->enemies_count; i++)
	{
		t_enemy *e = game->enemies + i;
		if (e->mad)
		{
			int dx = (game->player_x - e->x);
			if (dx > 0) dx = 1;
			else if (dx < 0) dx = -1;
			if (dx > 0)
				e->dx = 1;
			else if (dx < 0)
				e->dx = -1;


			int dy = (game->player_y - e->y);
			if (dy > 0) dy = 1;
			else if (dy < 0) dy = -1;
			if (dy > 0)
				e->dy = 1;
			else if (dy < 0)
				e->dy = -1;
			update_dir(game, &e->visual_x, dx, &e->vel_x, &e->x, e->y, 1, dx * 150);
			if (fabsf(e->visual_x - e->x) > 0.01f)
				dy = 0;
			update_dir(game, &e->visual_y, dy, &e->vel_y, &e->y, e->x, 0, dy * 150);
		}
		else
		{
			int dx = 0, dy = 0;
			update_dir(game, &e->visual_x, dx, &e->vel_x, &e->x, e->y, 1, dx * 150);
			update_dir(game, &e->visual_y, dy, &e->vel_y, &e->y, e->x, 0, dy * 150);
		}
	}
	//memset(game->draw_image.pixels, 0, game->draw_image.line_length * game->draw_image.height);
	//draw_rect(&game->draw_image, 0, 0, game->draw_image.width, game->draw_image.height, 0xff000000);
	//memset(game->light_image.pixels, 0, game->light_image.height * game->light_image.line_length);
	memcpy(game->draw_image.pixels, game->back_ground.pixels, game->draw_image.line_length * game->draw_image.height);
	
#if 1
	for (int y = 0; y < game->light_image.height; y++)
	{
		for (int x = 0; x < game->light_image.width; x++)
		{
			unsigned int *pixel = (unsigned int *)(game->light_image.pixels + y * game->light_image.line_length +
					x * (game->light_image.bits_per_pixel / 8));
			//*pixel = 0xff222222;
			//*pixel = 0xff555555;
			*pixel = 0xff000000;
		}
	}
#endif
	static float camera_x = 0;
	static float camera_y = 0;

#if SCROLL
	camera_x += (game->player_visual_x - camera_x) * 0.1f;
	camera_y += (game->player_visual_y - camera_y) * 0.1f;
#else
	camera_x = game->map.width  * 0.5f - 0.5f;	
	camera_y = game->map.height * 0.5f - 0.5f;
#endif

	float window_center_x = game->draw_image.width * 0.5f;
	float window_center_y = game->draw_image.height * 0.5f;
#if SCROLL
	for (int dy = -50; dy < 50; dy++) // min, max of map width + calculate this number
	{
		for (int dx = -50; dx < 50; dx++) // this should be inddependant of update
#else
	for (int x = 0; x < game->map.width; x++) // min, max of map width + calculate this number
	{
		for (int y = 0; y < game->map.height; y++) // this should be inddependant of update
#endif
		{
#if SCROLL
			int x = (game->player_x + dx);
			int y = (game->player_y + dy);
#endif
			float center_x = window_center_x + 
					(x - camera_x) * game->cell_dim;
			float center_y = window_center_y +
					(y - camera_y) * game->cell_dim;
			float min_x = center_x - 0.5f * game->cell_dim;
			float min_y = center_y - 0.5f * game->cell_dim;
			min_x = floorf(min_x);
			min_y = floorf(min_y);

			float max_x = min_x + game->cell_dim;
			float max_y = min_y + game->cell_dim;
			if (x >= 0 && x < game->map.width && y >= 0 && y < game->map.height)
			{
				char c = game->map.contents[y][x];
				if (c == 'E')
				{
					if (game->collected_count == game->map.collectibles_count)
					{
						//add_light_circle(game, center_x, center_y, game->cell_dim * 4, 0xffffffff);
						for (int i = 0; i < 5; i++)
						{
								if (game->particule_count >= array_length(game->particules))
									break;

								float base_x = (x) * game->cell_dim;
								float base_y = (y) * game->cell_dim;
								t_particule *p = &game->particules[game->particule_count++];

								p->x = base_x + (rand() % 20 - 10);
								p->y = base_y + (rand() % 20);
								p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
								p->dy = ((float)rand() / RAND_MAX)  ;

								float l = sqrtf(p->dx * p->dx + p->dy + p->dy);
								if (l > 0.1)
								{
									p->dx /= l;
									p->dy /= l;
								}
								p->lifetime = ((float)rand() / RAND_MAX) * 1.5f;
								p->lifetime_left = p->lifetime;
								p->size = rand() % 3 + 1;
								p->r = 1, p->g = 1, p->b = 1;
						}
					}
				}
				else if (c == 'C')
				{
					for (int i = 0; i < 1; i++)
					{
							if (game->particule_count >= array_length(game->particules))
								break;

							float base_x = (x ) * game->cell_dim;
							float base_y = (y) * game->cell_dim;
							t_particule *p = &game->particules[game->particule_count++];

							p->x = base_x + (rand() % (game->cell_dim * 2) - game->cell_dim);
							p->y = base_y + (rand() % (game->cell_dim * 2) - game->cell_dim);
							p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
							p->dy = ((float)rand() / RAND_MAX) * 2 - 1;

							float l = sqrtf(p->dx * p->dx + p->dy + p->dy);
							if (l > 0.1)
							{
								p->dx /= l;
								p->dy /= l;
							}
							p->lifetime = ((float)rand() / RAND_MAX) * .5f;
							p->lifetime_left = p->lifetime;
							if (game->cell_dim < 5)
								p->size = 1;
							else
								p->size = rand() % (game->cell_dim / 5) + 1;
							p->r = 1, p->g = 1, p->b = 0;
					}
					
					//add_light_circle(game, center_x, center_y, game->cell_dim * 10, 0xffffffff);
					draw_image(&game->draw_image, &game->coin[(game->coin_frame / (4 * game->map.collectibles_count)) % 4], min_x, min_y, max_x, max_y); 
					//draw_rect_outline(&game->draw_image, min_x, min_y, max_x, max_y, 1, 0xff00ffff);
					game->coin_frame++;
					continue;
				}
				//draw_rect_outline(&game->draw_image, min_x, min_y, max_x, max_y, 1, 0xff00ffff);
			}

		}
	}
	int target_dx = game->player_dx;
	int target_dy = game->player_dy;
	if (target_dx || target_dy)
	{
		for (int i = 0; i < 3; i++) // frame-rate dependant
		{
			if (game->particule_count >= array_length(game->particules))
				break;

			float base_x = game->player_visual_x * game->cell_dim;
			float base_y = game->player_visual_y * game->cell_dim;
			t_particule *p = &game->particules[game->particule_count++];
			if (target_dx)
			{
				p->x = base_x - target_dx * (rand() % game->cell_dim);
				p->y = base_y + (rand() % game->cell_dim - game->cell_dim / 2);
				p->dx = -target_dx;
				p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
			}
			else
			{
				p->y = base_y - target_dy * (rand() % game->cell_dim);
				p->x = base_x + (rand() % game->cell_dim - game->cell_dim / 2);
				p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
				p->dy = -target_dx;
			}
			p->lifetime = ((float)rand() / RAND_MAX) * 1.f;
			p->lifetime_left = p->lifetime;
			if (game->cell_dim < 5)
				p->size = 1;
			else
				p->size = rand() % (game->cell_dim / 5) + 1;
			if (rand() % 3)
				p->r = 0, p->g = 1, p->b = 1;
			else
				p->r = 1, p->g = 0.64, p->b = 0;
		}
	}
	for (int i = 0; i < game->enemies_count; i++)
	{
		t_enemy *e = game->enemies + i;
		if (e->mad)
		{
			int target_dx = e->dx;
			int target_dy = e->dy;
			for (int j = 0; j < 2; j++) // frame-rate dependant
			{
				if (game->particule_count >= array_length(game->particules))
					break;

				float base_x = e->visual_x * game->cell_dim;
				float base_y = e->visual_y * game->cell_dim;
				t_particule *p = &game->particules[game->particule_count++];
				if (target_dx)
				{
					p->x = base_x - target_dx * (rand() % game->cell_dim);
					p->y = base_y + (rand() % game->cell_dim - game->cell_dim / 2);
					p->dx = -target_dx;
					p->dy = ((float)rand() / RAND_MAX) * 2 - 1;
				}
				else
				{
					p->y = base_y - target_dy * (rand() % game->cell_dim);
					p->x = base_x + (rand() % game->cell_dim - game->cell_dim / 2);
					p->dx = ((float)rand() / RAND_MAX) * 2 - 1;
					p->dy = -target_dx;
				}
				p->lifetime = ((float)rand() / RAND_MAX) * 1.f;
				p->lifetime_left = p->lifetime;
				if (game->cell_dim < 5)
					p->size = 1;
				else
					p->size = rand() % (game->cell_dim / 5) + 1;
				p->r = 1, p->g = 0, p->b = 0;
			}
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
			unsigned int color = ((unsigned)(t * 255 + 0.5f) << 24)
								|((unsigned)(p->r * 255 + 0.5f) << 16)
								|((unsigned)(p->g * 255 + 0.5f) <<  8)
								|((unsigned)(p->b * 255 + 0.5f) <<  0);

			draw_rect(&game->draw_image, min_x, min_y, min_x + size, min_y + size, color);
			i++;
		}
		p->lifetime_left -= dt;
	}
	float min_x, min_y;
	min_x = window_center_x + (game->player_visual_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->player_visual_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	//draw_rect(&game->draw_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim, 0xffffff00);

	t_image *img;
	if (game->player_running)
		img = &game->player_run[(game->player_frame / 4) % 4][game->player_dir];
	else
		img = &game->player_idle[(game->player_frame / 4) % 4][game->player_dir];
	game->player_frame++;
	draw_image(&game->draw_image, img, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim);

	add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffffff);
	
	for (int i = 0; i < game->enemies_count; i++)
	{
		t_enemy *e = game->enemies + i;
		e->dir = (e->dx < 0);
		e->running = e->mad;
		if (e->running)
			img = &game->enemy_run[(e->frame / 4) % 4][e->dir];
		else
			img = &game->enemy_idle[(e->frame / 8) % 4][e->dir];
		e->frame++;

		min_x = window_center_x + (e->visual_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
		min_y = window_center_y + (e->visual_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;

		//draw_circle_outline(&game->draw_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, 5 * game->cell_dim, 3, 0xffffff);
		e->mad = dist_sq(e->visual_x, e->visual_y, game->player_visual_x, game->player_visual_y) < 5 * 5;

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
		add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, c);
		draw_image(&game->draw_image, img, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim);
	}
	//draw_rect(&game->draw_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim, 0xff005511);
	//add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 5, 0xff888888);

	min_x = window_center_x + (1 - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->map.height - 1 - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	//add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 20, 0xff22ff22);

	min_x = window_center_x + (game->map.width - 1 - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->map.height - 1 - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	//add_light_circle(game, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 20, 0xffff2222);
#if 1
	char *row = game->draw_image.pixels;
	float x_div = 1.0f / game->draw_image.width;
	float y_div = 1.0f / game->draw_image.height;
	float one_over_255 = 1.0f / 255;
	for (int y = 0; y < game->draw_image.height; y++)
	{
		unsigned *pixel = row;
		for (int x = 0; x < game->draw_image.width; x++)
		{
			int ix = x * x_div * game->light_image.width;
			int iy = y * y_div * game->light_image.height;
			unsigned int *l = (unsigned int *)(game->light_image.pixels
					+ iy * game->light_image.line_length
					+ ix * 4);
			int c = *l, p = *pixel;
			int cr = (c >> 16) & 0xFF, cg = (c >> 8) & 0xFF, cb = c & 0xFF;
			int pr = (p >> 16) & 0xFF, pg = (p >> 8) & 0xFF, pb = p & 0xFF;

			pr *= cr * one_over_255;
			pg *= cg * one_over_255;
			pb *= cb * one_over_255;

			*pixel = (pr << 16) | (pg << 8) | pb;
			pixel++;
		}
		row += game->draw_image.line_length;
	}
#endif

	draw_rect(&game->stat_image, 0, 0, game->stat_image.width, game->stat_image.height, 0xff555555);
	
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
				color = 0xff0000ff;
			else if (c == 'P')
				color = 0xffffff00;
			else if (c == 'X')
				color = 0xff005511;
			else if (c == 'E')
				color = (game->collected_count == game->map.collectibles_count ? 0xffff0000 : 0xffff00ff);
			else if (c == 'C')
				color = 0xff5f0f0f;
			draw_rect(&game->stat_image, min_x, min_y, min_x + dim, min_y + dim, color);
		}
	}
	clock_t curr = clock() - t;
	double target_seconds_per_frame = dt;
	char s[256];
	sprintf(s, "SCORE: %d\ntime: %2.fms", game->collected_count, ((double)curr / CLOCKS_PER_SEC) * 1000.0f);
	while ((double)curr / CLOCKS_PER_SEC < target_seconds_per_frame)
		curr = clock() - t;

	draw_text(&game->draw_image, s, 0, 0, 0xffffff);
	mlx_put_image_to_window(game->mlx, game->window, game->draw_image.img, 0, 0);
	return (0);
}

typedef struct zf_header {
	int width;
	int height;
	int count;
}zf_header;

int main(int argc, char **argv)
{

	//draw like a background with all the lights you want  and then the problem is just the enemies
	
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
	int window_width = 1920;
	int window_height = 1080;
	game.mlx = mlx_init();
	game.window = mlx_new_window(game.mlx, window_width, window_height, "so_long");

	game.draw_image.width = window_width;
	game.draw_image.height = window_height;




#if SCROLL
	if (game.draw_image.width < game.draw_image.height)
		game.cell_dim = game.draw_image.width / 16;
	else
		game.cell_dim = game.draw_image.height / 16;

#else
	int c1 = (game.draw_image.width) / game.map.width;
	int c2 = (game.draw_image.height) / game.map.height;
	game.cell_dim = c1;
	if (c2 < c1)
		game.cell_dim = c2;
	assert(game.cell_dim * game.map.width <= game.draw_image.width);
	assert(game.cell_dim * game.map.height <= game.draw_image.height);
#endif
	printf("%d %d %d\n", game.cell_dim, game.map.width, game.draw_image.width);
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

	game.light_scale = 1;
	game.light_image.width = game.draw_image.width * game.light_scale;
	game.light_image.height = game.draw_image.height * game.light_scale;
	game.light_image.line_length = game.light_image.width * 4;
	game.light_image.bits_per_pixel = 32;
	game.light_image.pixels = calloc(game.light_image.line_length * game.light_image.height, 1);

	assert(game.window && game.draw_image.img);

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
			if (game.map.contents[y][x] == 'X')
				game.enemies_count++;
		}
	}

	game.enemies = calloc(game.enemies_count, sizeof(t_enemy));
	int e = 0;
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
				game.enemies[e].visual_y = game.enemies[e].y = y;
				game.enemies[e].visual_x = game.enemies[e].x = x;
				e++;
			}
		}
	}



	printf("%d %d\n", game.map.collectibles_count, game.enemies_count);
	int fd = open("file.text", O_RDONLY);
	if (fd > 0)
	{
		zf_header h;
		read(fd, &h, sizeof(h));
		font_advance_x = h.width;
		font_line_height = h.height;
		for (int c = 32; c < 127; c++)
		{
			font_bitmaps[c] = calloc(h.width * h.height, sizeof(uint8_t));
			if (read(fd, font_bitmaps[c], h.width * h.height) < h.width * h.height)
				printf("error\n");
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
	}
	game.hole = load_image("hole.png");
	game.door[0] = load_image("doors_leaf_closed.png");
	game.door[1] = load_image("doors_leaf_open.png");
	game.dungeon_image = load_image("dungeon.png");
	game.ground_image = load_image("ground.png");
	game.wall_image = load_image("wall.png");


	game.back_ground.width = game.draw_image.width;
	game.back_ground.height = game.draw_image.height;
	game.back_ground.line_length = game.back_ground.width * 4;
	game.back_ground.bits_per_pixel = 32;
	game.back_ground.pixels = calloc(1, game.back_ground.line_length * game.back_ground.height);
	for (int x = 0; x < game.map.width; x++)
	{
		for (int y = 0; y < game.map.height; y++)
		{
			float camera_x = game.map.width  * 0.5f - 0.5f;	
			float camera_y = game.map.height * 0.5f - 0.5f;
			char c = game.map.contents[y][x];
			float window_center_x = game.back_ground.width * 0.5f;
			float window_center_y = game.back_ground.height * 0.5f;
			float center_x = window_center_x + 
					(x - camera_x) * game.cell_dim;
			float center_y = window_center_y +
					(y - camera_y) * game.cell_dim;
			float min_x = center_x - 0.5f * game.cell_dim;
			float min_y = center_y - 0.5f * game.cell_dim;
			min_x = floorf(min_x);
			min_y = floorf(min_y);//??

			float max_x = min_x + game.cell_dim;
			float max_y = min_y + game.cell_dim;
			int p = game.floors[y * game.map.width + x];
			draw_image(&game.back_ground, &game.floor[p], min_x, min_y, max_x, max_y);
				if (c == '1')
				{
					t_image img = game.dungeon_image;
					img.width = 16;
					img.height = 16;
					int px, py;
					if (!x)
						px = 0, py = y % 4;
					else if (x == game.map.width - 1)
						px = 5, py = y % 4;
					else if (!y)
						py = 0, px = 1 + x % 4;
					else if (y == game.map.height - 1)
						py = 4, px = 1 + x % 4;
					else
					{
						draw_image(&game.back_ground, &game.hole, min_x, min_y, max_x, max_y);
						continue;
					}
					img.pixels = (game.dungeon_image.pixels + py * 16 * img.line_length + px * 16 * 4);
					draw_image(&game.back_ground, &img, min_x, min_y, max_x, max_y);
				}
				else if (c == 'E')
				{
					if (game.collected_count < game.map.collectibles_count)
					{
						draw_image(&game.back_ground, &game.door[0], min_x, min_y, max_x, max_y);	
					}
					else
					{
						//add_light_circle(game, center_x, center_y, game.cell_dim * 4, 0xffffffff);
						draw_image(&game.back_ground, &game.door[1], min_x, min_y, max_x, max_y);	
					}
				}
				else if (c == 'C')
				{
					//add_light_circle(game, center_x, center_y, game.cell_dim * 5, 0xffffffff);
					//draw_image(&game.back_ground, &game.coin[(game.coin_frame / (4 * game.map.collectibles_count)) % 4], min_x, min_y, max_x, max_y); 
				}
		}
	}
	mlx_do_key_autorepeatoff(game.mlx);
	// read png in another program and output a simple format (with alpha)
#if 0
	game.player_image.img = mlx_xpm_file_to_image(game.mlx, "Idle.xpm", &game.player_image.width, &game.player_image.height);
	game.player_image.pixels = mlx_get_data_addr(game.player_image.img, 
	 	&game.player_image.bits_per_pixel, 
	 	&game.player_image.line_length, 
	 	&game.player_image.endian);
#endif
	
	mlx_hook(game.window, 2, 0, on_key_down, &game);
	mlx_hook(game.window, 3, 0, on_key_up, &game);
	mlx_hook(game.window, 17, 0, exit_game, &game);
	mlx_loop_hook(game.mlx, loop_hook, &game);
	mlx_loop(game.mlx);
}
