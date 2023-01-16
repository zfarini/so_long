/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/15 17:57:52 by zfarini           #+#    #+#             */
/*   Updated: 2023/01/16 12:54:50 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_H
# define SO_LONG_H
# include <mlx.h>
# include <stdio.h>
# include <fcntl.h>
# include "libft/libft.h"
# include <string.h>
# include <math.h>

/* 1.0f / 255 */
# define ONE_OVER_255 0.00392156862f

enum {
	ESCAPE = 53,
	KEY_UP = 13,
	KEY_DOWN = 1,
	KEY_LEFT = 0,
	KEY_RIGHT = 2,
};

typedef struct s_draw_info {
	int				min_x;
	int				min_y;
	int				max_x;
	int				max_y;
	float			x_c;
	float			y_c;
	unsigned int	src;
	int				dist_sq;
	float			t;
	unsigned int	*dest;
	unsigned int	*dest2;
	int				x;
	int				y;
	unsigned int	color;
}	t_draw_info;

typedef struct s_color_rgb {
	int	r;
	int	g;
	int	b;
}	t_color_rgb;

typedef struct s_map {
	int		player_pos;
	int		p_count;
	int		e_count;
	int		fd;
	int		width;
	int		height;
	int		collectibles_count;
	char	**arr;
	int		*visited;
}	t_map;

typedef struct s_image {
	int		width;
	int		height;
	int		line_length;
	int		bits_per_pixel;
	int		endian;
	char	*pixels;
	void	*img;
}	t_image;

typedef struct s_game {
	void		*mlx;
	void		*window;
	t_map		original_map;
	t_map		map;
	int			cell_dim;
	int			offset_x;
	int			offset_y;
	int			moves_count;
	int			collected_count;
	int			*floors;
	int			window_scale;
	t_image		background;
	t_image		window_image;
	t_image		draw_image;
	t_image		death_image;
	t_image		wall_top;
	t_image		wall_bottom;
	t_image		wall_left;
	t_image		wall_right;
	t_image		floor_ladder;
	t_image		floor[8];
	t_image		door[2];
	t_image		player_idle[4][2];
	t_image		player_run[4][2];
	t_image		enemy_idle[4][2];
	t_image		enemy_run[4][2];
	t_image		torch[4];
	t_image		coin[4];
	t_image		hole;
	char		*images_pixels[128];
	int			images_count;
	int			player_dir;
	int			player_y;
	int			player_x;
	int			player_dx;
	int			player_dy;
	int			data_read_fd;
}	t_game;

/*background.c*/
void			init_background(t_game *game);
/*game_restart.c*/
void			restart_game(t_game *game);
/*draw_map.c*/
void			draw_map(t_game *game);
/*game_init.c*/
void			init_game(t_game *game, char *map_file);
int				string_ends_with(char *s, char *ext);
/*player.c*/
void			update_and_draw_player(t_game *game);
/*load_images.c*/
void			load_all_images(t_game *game);
/*draw.c*/
void			draw_image(t_game *game, t_image *image, int min_x, int min_y);
void			draw_to_window_image(t_game *game);
/*utils.c*/
void			*ft_alloc(t_game *game, size_t size);
void			exit_game(t_game *game, int failed);
unsigned int	lerp_color(unsigned color1, unsigned color2, float t);
float			clamp(float x, float min, float max);
void			get_move_count(t_game *game, char *s);
/*map_utils.c*/
void			map_error(t_map *map, char *s);
char			*append_char(t_map *map, char *s, char c);
char			*get_next_line(t_map *map, int fd);
void			*ft_realloc(t_map *map, void *ptr,
					size_t old_size, size_t new_size);
int				check_if_string_have_one_unique_char(char *s, char c);
/*map.c*/
void			parse_map(t_map *map, char *map_filename);
#endif
