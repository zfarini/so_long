#ifndef SO_LONG_H
#define SO_LONG_H
# include "mlx/mlx.h"
# include <assert.h>
# include <stdio.h>
# include <fcntl.h>
# include "libft/libft.h"

typedef struct s_map {
	int		width;
	int		height;
	int		collectibles_count;
	char	**contents;//todo: store exit and start and collectibles positino
}	t_map;

typedef struct s_image {
	int		width;
	int		height;
	int		line_length;;
	int		endian;
	int		bits_per_pixel;
	char	*pixels;
	void	*img;
}	t_image;

typedef struct s_particule {
	float x, y;
	float dx, dy;
	float size;
	float lifetime_left;
	float lifetime;
	float r, g, b;
}	t_particule;


typedef struct s_game{
	void	*mlx;
	void	*window;
	t_image draw_image;
	t_image	light_image;
	t_image stat_image;
	t_image	text_image;
	int		cell_dim;
	int		player_y;
	int		player_x;
	float	player_visual_x;
	float	player_visual_y;

	float vel_x;
	float vel_y;


	t_particule particules[4096];
	int particule_count;

	int move_y;

	int player_dx;
	int player_dy;

	int enemy_x;
	int enemy_y;
	float enemy_visual_x;
	float enemy_visual_y;
	int enemy_dx;

	t_map	map;
	int		moves_count;
	int		collected_count;
	t_image player_image;
}	t_game;



int	parse_map(t_map *map, char *map_filename);
#endif
