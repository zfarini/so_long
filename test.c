
void init_game(t_game *game, t_map *map)
{
	memset(game, 0, sizeof(*game);
	game->map = *map;
	game->map.contents = malloc(map->height * sizeof(char *));
	for (int i = 0; i < map->height; i++)
		game->map.contents[i] = strdup(map->contents[i]);
	game->draw_image.width = WINDOW_WIDTH;
	game->draw_image.height = WINDOW_HEIGHT;

	int c1 = (game->draw_image.width) / game->map.width;
	int c2 = (game->draw_image.height) / game->map.height;
	game->cell_dim = c1;
	if (c2 < c1)
		game->cell_dim = c2;
	assert(game->cell_dim * game->map.width <= game->draw_image.width);
	assert(game->cell_dim * game->map.height <= game->draw_image.height);
	game->draw_image.img = mlx_new_image(game->mlx, window_width, window_height);
	game->draw_image.pixels = mlx_get_data_addr(game->draw_image.img, 
		&game->draw_image.bits_per_pixel, 
		&game->draw_image.line_length, 
		&game->draw_image.endian);

	game->stat_image.width = window_width - game->draw_image.width;
	game->stat_image.height = window_height;
	game->stat_image.bits_per_pixel = game->draw_image.bits_per_pixel;
	game->stat_image.line_length = game->draw_image.line_length;
	game->stat_image.pixels = game->draw_image.pixels + game->draw_image.width * (game->draw_image.bits_per_pixel / 8);

	game->light_scale = .125f;
	game->light_image.width = game->draw_image.width * game->light_scale;
	game->light_image.height = game->draw_image.height * game->light_scale;
	game->light_image.line_length = game->light_image.width * 4;
	game->light_image.bits_per_pixel = 32;
	game->light_image.pixels = calloc(game->light_image.line_length * game->light_image.height, 1);

	assert(game->window && game->draw_image.img);

	game->floors = malloc(game->map.width * game->map.height * sizeof(int));
	for (int y = 0; y < game->map.height; y++)
	{
		for (int x = 0; x < game->map.width; x++)
		{
			int p = rand() % 4;
			if (!(rand() % 5))
			{
				p = rand() % 3 + 4;
			}
			game->floors[y * game->map.width + x] = p;
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

	printf("%d %d\n", game->map.collectibles_count, game->enemies_count);
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
		game->player_idle[i][0] = load_image(s);
		sprintf(s, "knight_m_run_anim_f%d.png", i);
		game->player_run[i][0] = load_image(s);
#if 1
		game->player_run[i][0].height -= 7;
		game->player_run[i][0].pixels += 7 * game->player_idle[i][0].line_length;
		game->player_idle[i][0].height -= 7;
		game->player_idle[i][0].pixels += 7 * game->player_idle[i][0].line_length; 

#endif
		game->player_idle[i][1] = flip_image_by_x(&game->player_idle[i][0]);
		game->player_run[i][1] = flip_image_by_x(&game->player_run[i][0]);

		sprintf(s, "big_demon_idle_anim_f%d.png", i);
		game->enemy_idle[i][0] = load_image(s);
		sprintf(s, "big_demon_run_anim_f%d.png", i);
		game->enemy_run[i][0] = load_image(s);

		game->enemy_run[i][0].height -= 3;
		game->enemy_run[i][0].pixels += 3 * game->enemy_idle[i][0].line_length;
		game->enemy_idle[i][0].height -= 3;
		game->enemy_idle[i][0].pixels += 3 * game->enemy_idle[i][0].line_length; 

		game->enemy_idle[i][1] = flip_image_by_x(&game->enemy_idle[i][0]);
		game->enemy_run[i][1] = flip_image_by_x(&game->enemy_run[i][0]);

		sprintf(s, "coin_anim_f%d.png", i);
		game->coin[i] = load_image(s);
		
	}
	for (int i = 0; i < 8; i++)
	{
		sprintf(s, "floor_%d.png", i + 1);
		game->floor[i] = load_image(s);
	}
	game->hole = load_image("hole.png");
	game->door[0] = load_image("doors_leaf_closed.png");
	game->door[1] = load_image("doors_leaf_open.png");
	game->wall_top_left = load_image("wall_top_left.png");
	game->wall_top_right = load_image("wall_top_right.png");
	game->wall_bottom_left = load_image("wall_bottom_left.png");
	game->wall_bottom_right = load_image("wall_bottom_right.png");
	game->wall_top = load_image("wall_top.png");
	game->wall_bottom = load_image("wall_bottom.png");
	game->wall_left = load_image("wall_left.png");
	game->wall_right = load_image("wall_right.png");
	game->wall_corner = game->wall_bottom;
	game->wall_corner.width = 4;
	game->floor_ladder = load_image("floor_ladder.png");
	game->death_image = load_image("death.png");


	game->back_ground.width = game->draw_image.width;
	game->back_ground.height = game->draw_image.height;
	game->back_ground.line_length = game->back_ground.width * 4;
	game->back_ground.bits_per_pixel = 32;
	game->back_ground.pixels = calloc(1, game->back_ground.line_length * game->back_ground.height);
	for (int x = 0; x < game->map.width; x++)
	{
		for (int y = 0; y < game->map.height; y++)
		{
			float camera_x = game->map.width  * 0.5f - 0.5f;	
			float camera_y = game->map.height * 0.5f - 0.5f;
			char c = game->map.contents[y][x];
			float window_center_x = game->back_ground.width * 0.5f;
			float window_center_y = game->back_ground.height * 0.5f;
			float center_x = window_center_x + 
					(x - camera_x) * game->cell_dim;
			float center_y = window_center_y +
					(y - camera_y) * game->cell_dim;
			float min_x = center_x - 0.5f * game->cell_dim;
			float min_y = center_y - 0.5f * game->cell_dim;
			min_x = floorf(min_x);
			min_y = floorf(min_y);//??

			float max_x = min_x + game->cell_dim;
			float max_y = min_y + game->cell_dim;
			int p = game->floors[y * game->map.width + x];
			//todo: render something under the door (checkout small map)
			if (c == '1')
			{
				t_image *img = &game->hole;

#if 0
				if (!x && !y) // top-left
				{
				}
				else if (x == game->map.width - 1 && !y) // top - right
				{

				}
				else if (!x && y == game->map.height - 1) // bottom - left
				{

				}
				else if (x == game->map.width - 1 && y == game->map.height - 1) // bottom-right
				{

				}
#endif
				if (!x) // left
				{
					img = &game->wall_left;
				}
				else if (x == game->map.width - 1) // right
				{
					img = &game->wall_right;
				}
				else if (!y) // top
				{
					img = &game->wall_top;
				}
				else if (y == game->map.height - 1) // bottom
				{
					img = &game->wall_bottom;
					min_y -= game->cell_dim * 0.75;
					max_y -= game->cell_dim * 0.75;
				}
				draw_image(&game->back_ground, img, min_x, min_y, max_x, max_y);
			}
			else if (c == 'E')
				draw_image(&game->back_ground, &game->floor_ladder, min_x, min_y, max_x, max_y);
			else
				draw_image(&game->back_ground, &game->floor[p], min_x, min_y, max_x, max_y);

			min_x = center_x - 0.5f * game->cell_dim;
			min_y = center_y - 0.5f * game->cell_dim;
			min_x = floorf(min_x);
			min_y = floorf(min_y);
			max_x = min_x + game->cell_dim;
			max_y = min_y + game->cell_dim;
			draw_rect_outline(&game->back_ground, min_x, min_y, max_x, max_y, 1, 0xffffffff);
		}
	}

}
