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
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define USE_SRGB 1

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
			uint *src =  (uint *)(image->pixels + iy * image->line_length +
			ix * (image->bits_per_pixel / 8));

			uint  *dest = (uint *)(draw_image->pixels + y * draw_image->line_length +
				x * (draw_image->bits_per_pixel / 8));
#if 0
			int src_r = (*src >> 16) & 0xFF, dest_r = (*dest >> 16) & 0xFF;
			int src_g = (*src >> 8) & 0xFF, dest_g = (*dest >> 8) & 0xFF;
			int src_b = (*src >> 0) & 0xFF, dest_b = (*dest >> 0) & 0xFF;
			float t = ((*src >> 24) & 0xFF) / 255.0f;
			int r = (1 - t) * dest_r + t * src_r;
			int g = (1 - t) * dest_g + t * src_g;
			int b = (1 - t) * dest_b + t * src_b;
			*dest = (r << 16) | (g << 8) | b;
#else
			*dest = (*src & 0xFFFFFF);
#endif

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
	draw_rect(&game->draw_image, 0, 0, game->draw_image.width, game->draw_image.height, 0xff000000);
	for (int y = 0; y < game->light_image.height; y++)
	{
		for (int x = 0; x < game->light_image.width; x++)
		{
			unsigned int *pixel = (unsigned int *)(game->light_image.pixels + y * game->light_image.line_length +
					x * (game->light_image.bits_per_pixel / 8));
			*pixel = 0;
			*pixel = 0xff222222;
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
			min_x = floorf(min_x);
			min_y = floorf(min_y);

			float max_x = min_x + game->cell_dim;
			float max_y = min_y + game->cell_dim;
			if (x >= 0 && x < game->map.width && y >= 0 && y < game->map.height)
			{
				char c = game->map.contents[y][x];
				unsigned int color = 0;
				if (c == '1')
				{
					t_image img = game->wall_image;
					if (!x || x == game->map.width - 1)
					{
						img.width = 6;
						img.height = 90;
						img.pixels = (img.pixels + 32 * img.line_length + 289 * 4); 
					}
					else if (!y || y == game->map.height - 1)
					{
						img.width = 90;
						img.height = 6;
						img.pixels = (img.pixels + 33 * img.line_length + 389 * 4);
					}
					else
					{
						img.width = 66;
						img.height = 70;
						//img.pixels = (img.pixels + 290 * img.line_length + 30 * 4); 

						img.width = 6;
						img.height = 90;
						img.pixels = (img.pixels + 32 * img.line_length + 289 * 4); 
					}
					draw_image(&game->draw_image, &img, min_x, min_y, max_x, max_y);
					continue;
				}
				else if (c == 'E')
					color = (game->collected_count == game->map.collectibles_count ? 0xffff0000 : 0xff555555);
				else if (c == 'C')
					color = 0xff5f0f0f;
				else
				{
					x %= 16;
					y %= 16;
					t_image img = game->ground_image;
					img.width = 16;
					img.height = 16;
					img.pixels = (img.pixels + y * 16 * img.line_length + x * 16 * (img.bits_per_pixel / 8));

					draw_image(&game->draw_image, &img, min_x, min_y, max_x, max_y);
					continue;
				}
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
			unsigned int color = ((unsigned)(t * 255 + 0.5f) << 24)
								|((unsigned)(p->r * 255 + 0.5f) << 16)
								|((unsigned)(p->g * 255 + 0.5f) <<  8)
								|((unsigned)(p->b * 255 + 0.5f) <<  0);

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
	float min_x, min_y;
	min_x = window_center_x + (game->player_visual_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->player_visual_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	draw_rect(&game->draw_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim, 0xffffff00);
	//draw_image(&game->draw_image, &game->player_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim);

	add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffffff);

	min_x = window_center_x + (game->player_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->player_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	
	min_x = window_center_x + (game->enemy_visual_x - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->enemy_visual_y - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	draw_rect(&game->draw_image, min_x, min_y, min_x + game->cell_dim, min_y + game->cell_dim, 0xff005511);
	//add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 5, 0xff888888);

	min_x = window_center_x + (0 - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->map.height - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	//add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffffff);

	min_x = window_center_x + (game->map.width - camera_x) * game->cell_dim - 0.5f * game->cell_dim;
	min_y = window_center_y + (game->map.height - camera_y) * game->cell_dim - 0.5f * game->cell_dim;
	//add_light_circle(&game->light_image, min_x + 0.5f * game->cell_dim, min_y + 0.5f * game->cell_dim, game->cell_dim * 10, 0xffffffff);
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

	draw_text(&game->stat_image, s, 0, offset_y + dim * game->map.height, 0xffffff);


#if 0
	t_image img = game->wall_image;
	img.width = 66;
	img.height = 70;
	img.pixels = (img.pixels + 290 * img.line_length + 30 * 4); 
	draw_image(&game->draw_image, &img, 0, 0, img.width, img.height);
#endif


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
	int window_height = 540;
	game.mlx = mlx_init();
	game.window = mlx_new_window(game.mlx, window_width, window_height, "so_long");

	game.draw_image.width = window_width * 0.8;
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
	game.dungeon_image = load_image("dungeon.png");
	game.ground_image = load_image("ground.png");
	game.wall_image = load_image("wall.png");

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
