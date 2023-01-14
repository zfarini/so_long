#include "so_long.h"
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int data_fd;
int data_read_fd;

t_image load_image(t_game *game, char *filename)
{
	(void)game;
#if 1
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
    img.pixels = (char *)pixels;//malloc(w * h * 4);
    //pixels are 0xRRGGBBAA
	//todo: n == 3?
	
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
			unsigned int *pixel = (unsigned int *)(img.pixels + y * img.line_length +
					x * 4);
            uint32_t p = *pixel;
            uint32_t r = (p >> 0)  & 0xFF;
            uint32_t g = (p >> 8)  & 0xFF;
            uint32_t b = (p >> 16) & 0xFF;
            uint32_t a = (p >> 24) & 0xFF;
            *pixel = (a << 24) | (r << 16) | (g << 8) | (b << 0);
            pixel++;
        }
    }
#else
	static int curr_img;
	t_image img;
	read(data_read_fd, &img.width, sizeof(img.width));
	read(data_read_fd, &img.height, sizeof(img.height));
	img.line_length = img.width * 4;
	img.pixels = malloc(img.width * img.height * sizeof(unsigned int));
	if (!img.pixels)
	{
		printf("Error\nmalloc failed\n");
		exit_game(game, 1);
	}
	images[curr_img++] = img.pixels;
	read(data_read_fd, img.pixels, img.width * img.height * sizeof(unsigned int));
#endif
#if 1
	{
		write(data_fd, &img.width, sizeof(img.width));
		write(data_fd, &img.height, sizeof(img.height));
		write(data_fd, img.pixels, img.width * img.height * sizeof(unsigned int));
	}
#endif
    return img;
}

t_image flip_image_by_x(t_game *game, t_image *image)
{
	t_image res;
	int y;
	int x;

	res = *image;
	res.pixels = malloc(res.height * res.line_length);
	if (!res.pixels)
	{
		printf("Error\nmalloc failed\n");
		exit_game(game, 1);
	}
	y = 0;
	while (y < res.height)
	{
		x = 0;
		while (x < res.width)
		{
			*((unsigned int *)(res.pixels + y * res.line_length +
					x * 4)) = 
				*((unsigned int *)(image->pixels + y * image->line_length +
					(image->width - x - 1) * 4));
			x++;
		}
		y++;
	}
	return (res);
}


void restart_game(t_game *game)
{
	int	i;
	int y;
	int x;
	int e;

	if (game->map.contents)
	{
		i = 0;
		while (i < game->map.height)
			free(game->map.contents[i++]);
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
	if (!game->map.contents)
	{
		printf("Error\nmalloc failed\n");
		exit_game(game, 1);
	}
	i = 0;
	while (i < game->map.height)
	{
		game->map.contents[i] = strdup(game->original_map.contents[i]);
		i++;
	}
	game->enemies_count = 0;
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (game->map.contents[y][x] == 'X')
				game->enemies_count++;
			x++;
		}
		y++;
	}
	game->enemies = calloc(game->enemies_count, sizeof(t_enemy));
	if (!game->enemies)
	{
		printf("Error\nmalloc failed\n");
		exit_game(game, 1);
	}
	e = 0;
	y = 0;
	while(y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
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
			x++;
		}
		y++;
	}
}

void init_background_cell(t_game *game, int x, int y)
{
	int min_x;
	int min_y;
	t_image *img;

	min_x = game->offset_x + x * game->cell_dim;
	min_y = game->offset_y + y * game->cell_dim;
	if (game->map.contents[y][x] == '1')
	{
		img = 0;

		if ((!x && !y) || (x == game->map.width - 1 && !y) 
			|| (!x && y == game->map.height - 1)
			|| (x == game->map.width - 1 && y == game->map.height - 1))
		{
		}
		else if (!x)
			img = &game->wall_left;
		else if (x == game->map.width - 1)
			img = &game->wall_right;
		else if (!y)
			img = &game->wall_top;
		else if (y == game->map.height - 1)
		{
			img = &game->wall_bottom;
			min_y -= game->cell_dim * 0.75;
		}
		else
			img = &game->hole;
		if (img)
			draw_image(&game->back_ground, img, min_x, min_y, min_x + game->cell_dim,
					min_y + game->cell_dim);
	}
	else if (game->map.contents[y][x] == 'E')
		draw_image(&game->back_ground, &game->floor_ladder, min_x, min_y, 
				min_x + game->cell_dim, min_y + game->cell_dim);
	else
		draw_image(&game->back_ground,
				&game->floor[game->floors[y * game->map.width + x]],
				min_x, min_y,
				min_x + game->cell_dim, min_y + game->cell_dim);
#if 0
	min_x = game->offset_x + x * game->cell_dim;
	min_y = game->offset_y + y * game->cell_dim;
	max_x = min_x + game->cell_dim;
	max_y = min_y + game->cell_dim;
	draw_rect_outline(&game->back_ground, min_x, min_y, max_x, max_y, 1, 0xffffffff);
#endif
}

void init_background(t_game *game)
{
	int x;
	int y;

	game->back_ground.width = game->draw_image.width;
	game->back_ground.height = game->draw_image.height;
	game->back_ground.line_length = game->back_ground.width * 4;
	game->back_ground.pixels = ft_calloc(1, game->back_ground.line_length * game->back_ground.height);
	if (!game->back_ground.pixels)
	{
		printf("Error\nmalloc failed\n");
		exit_game(game, 1);
	}
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			init_background_cell(game, x, y);
			x++;
		}
		y++;
	}
}

void init_torch_place(t_game *game)
{
	int y;
	int x;
	int i;
	int j;
	int can;

	game->place_torch = calloc(game->map.width * game->map.height, sizeof(int));
	game->place_torch[0] = 1;
	game->place_torch[game->map.width - 2] = 1;
	game->place_torch[(game->map.height - 2) * game->map.width] = 1;
	game->place_torch[(game->map.height - 2) * game->map.width + game->map.width - 2] = 1;
	y = 0;
	while(y < game->map.height - 1)
	{
		x = 0;
		while (x < game->map.width - 1)
		{
			can = (x == 0 || x == game->map.width - 2 || y == 0 || y == game->map.height - 2);
			i = y - 4;
			while (i < y + 4)
			{
				j = x - 4;
				while (j < x + 4)
				{
					if (i >= 0 && j >= 0 && i < game->map.height && j < game->map.width &&
							game->place_torch[i * game->map.width + j])
						can = 0;
					j++;
				}
				i++;
			}
			if (can && rand() % 10 == 0)
			{
				game->place_torch[y * game->map.width + x] = 1;
			}
			x++;
		}
		y++;
	}
}

void init_floor(t_game *game)
{
	int	y;
	int	x;

	game->floors = malloc(game->map.width * game->map.height * sizeof(int));
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			int p = rand() % 4;
			if (!(rand() % 5))
			{
				p = rand() % 3 + 4;
			}
			game->floors[y * game->map.width + x] = p;
			x++;
		}
		y++;
	}
}

void load_player_images(t_game *game)
{
	char s[256];
	int	i;

	i = 0;
	while (i < 4)
	{
		sprintf(s, "knight_m_idle_anim_f%d.png", i);
		game->player_idle[i][0] = load_image(game, s);
		sprintf(s, "knight_m_run_anim_f%d.png", i);
		game->player_run[i][0] = load_image(game, s);
		game->player_run[i][0].height -= 7;
		game->player_run[i][0].pixels += 7 * game->player_idle[i][0].line_length;
		game->player_idle[i][0].height -= 7;
		game->player_idle[i][0].pixels += 7 * game->player_idle[i][0].line_length; 
		game->player_idle[i][1] = flip_image_by_x(game, &game->player_idle[i][0]);
		game->player_run[i][1] = flip_image_by_x(game, &game->player_run[i][0]);
		i++;
	}
}

void load_enemy_images(t_game *game)
{
	char s[256];
	int	i;

	i = 0;
	while (i < 4)
	{
		sprintf(s, "big_demon_idle_anim_f%d.png", i);
		game->enemy_idle[i][0] = load_image(game, s);
		sprintf(s, "big_demon_run_anim_f%d.png", i);
		game->enemy_run[i][0] = load_image(game, s);
		game->enemy_run[i][0].height -= 3;
		game->enemy_run[i][0].pixels += 3 * game->enemy_idle[i][0].line_length;
		game->enemy_idle[i][0].height -= 3;
		game->enemy_idle[i][0].pixels += 3 * game->enemy_idle[i][0].line_length; 
		game->enemy_idle[i][1] = flip_image_by_x(game, &game->enemy_idle[i][0]);
		game->enemy_run[i][1] = flip_image_by_x(game, &game->enemy_run[i][0]);
		i++;
	}
}

void load_all_images(t_game *game, char *data_file_path)
{
	char s[256];
	int	i;
	
	data_read_fd = open(data_file_path, O_RDONLY);
	if (data_read_fd < 0)
	{
		printf("Error\nfailed to open game data file\n");
		exit_game(game, 1);
	}
	load_player_images(game);
	load_enemy_images(game);
	i = 0;
	while (i < 8)
	{
		sprintf(s, "floor_%d.png", i + 1);
		game->floor[i] = load_image(game, s);
		if (i < 4)
		{
			sprintf(s, "coin_anim_f%d.png", i);
			game->coin[i] = load_image(game, s);
			sprintf(s, "torch_%d.png", i + 1);
			game->torch[i] = load_image(game, s);
		}
		i++;
	}
	game->hole = load_image(game, "hole.png");
	game->door[0] = load_image(game, "doors_leaf_closed.png");
	game->door[1] = load_image(game, "doors_leaf_open.png");
	game->wall_top = load_image(game, "wall_top.png");
	game->wall_bottom = load_image(game, "wall_bottom.png");
	game->wall_left = load_image(game, "wall_left.png");
	game->wall_right = load_image(game, "wall_right.png");
	game->floor_ladder = load_image(game, "floor_ladder.png");
	game->death_image = load_image(game, "death.png");
}

void init_offscreen_images(t_game *game)
{
	game->window_scale = 2;
	game->draw_image.width = WINDOW_WIDTH  / game->window_scale;
	game->draw_image.height = WINDOW_HEIGHT / game->window_scale;
	game->draw_image.line_length = game->draw_image.width * 4;
	game->draw_image.pixels = calloc(game->draw_image.line_length * game->draw_image.height, 1);
	game->window_image.width = WINDOW_WIDTH;
	game->window_image.height = WINDOW_HEIGHT;
	game->window_image.img = mlx_new_image(game->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	game->window_image.pixels = mlx_get_data_addr(game->window_image.img, 
		&game->window_image.bits_per_pixel, 
		&game->window_image.line_length, 
		&game->window_image.endian);
	game->light_scale = 0.25f;
	game->light_image.width = game->draw_image.width * game->light_scale;
	game->light_image.height = game->draw_image.height * game->light_scale;
	game->light_image.line_length = game->light_image.width * 4;
	game->light_image.pixels = calloc(game->light_image.line_length * game->light_image.height, 1);
}

void calc_cell_dim_and_offset(t_game *game)
{
	int c1;
	int c2;

	c1 = game->draw_image.width / game->map.width;
	c2 = game->draw_image.height / game->map.height;
	game->cell_dim = c1;
	if (c2 < c1)
		game->cell_dim = c2;
	game->offset_x = (game->draw_image.width - game->cell_dim * game->map.width) / 2;
	game->offset_y = (game->draw_image.height - game->cell_dim * game->map.height) / 2;
}

void init_game(t_game *game, char *map_file)
{
	memset(game, 0, sizeof(*game));
	if (!parse_map(&game->original_map, map_file))
		exit_game(game, 1);
	restart_game(game);
	game->mlx = mlx_init();
	if (!game->mlx)
	{
		printf("Error\nfailed to init mlx\n");
		exit_game(game, 1);
	}
	game->window = mlx_new_window(game->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "so_long");
	if (!game->window)
	{
		printf("Error\nfailed to create window\n");
		exit_game(game, 1);
	}
	init_offscreen_images(game);	
	calc_cell_dim_and_offset(game);

	data_fd = open("data.zf", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	load_all_images(game, "game_data.zf");
	init_floor(game);
	init_torch_place(game);
	init_background(game);
}
