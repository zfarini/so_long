#ifndef SO_LONG_H
#define SO_LONG_H
# include <mlx.h>
# include <assert.h>
# include <stdio.h>
# include <fcntl.h>
# include "libft/libft.h"

// remove all stuff dependant on pixels or frame rate (particules, animation ..)
// there's still a black line on the screen sometimes (float stuff?, keep walking on wall)
// if we got fixed camera then we can render the ground once (kinda)
//use for exit hole/png and floor_ladder.png ?
//play some effect when you get a coin
//place light things around

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

typedef struct s_enemy {
	int frame;
	int running;
	int dir;
	float  t;
	int mad;
	float vel_x;
	float vel_y;
	int x;
	int y;
	float visual_x;
	float visual_y;
	int dx;
	int dy;
}t_enemy;

typedef struct s_light {
	int cx, cy;
	int radius;
	int r, g, b;
}t_light;

typedef struct s_game{
	void	*mlx;
	void	*window;

	t_map	map;
	int		cell_dim;
	int		moves_count;
	int		collected_count;

	int *floors;

	float light_scale;

	t_image back_ground;

	t_image draw_image;
	t_image	light_image;
	t_image stat_image;
	t_image	text_image;
	t_image ground_image;
	t_image wall_image;
	t_image dungeon_image;

	t_image floor[8];
	t_image door[2];
	t_image player_idle[4][2];
	t_image player_run[4][2];
	t_image enemy_idle[4][2];
	t_image enemy_run[4][2];

	int light_count;
	t_light lights[512];

	t_image hole;

	int		player_frame; // frame-dependant
	int 	player_running;
	int 	player_dir; // 0 -> right, 1 -> left
	int		player_y;
	int		player_x;
	float	player_visual_x;
	float	player_visual_y;
	int player_dx;
	int player_dy;
	float vel_x;
	float vel_y;

	t_image coin[4];
	int coin_frame; // multiple coins!
	
	int enemies_count;
	t_enemy *enemies;

	t_particule particules[8192 * 2];
	int particule_count;
}	t_game;



int	parse_map(t_map *map, char *map_filename);
#endif
