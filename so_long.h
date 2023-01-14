#ifndef SO_LONG_H
#define SO_LONG_H
# include <mlx.h>
# include <assert.h>
# include <stdio.h>
# include <fcntl.h>
# include "libft/libft.h"
# include <string.h>
# include <math.h>
# include <time.h>

#define dt (1.0f / 30)

# define array_length(arr) ((int)(sizeof(arr) / sizeof(*arr)))
// remove all stuff dependant on pixels or frame rate (particules, animation ..)
// there's still a black line on the screen sometimes (float stuff?, keep walking on wall)
// if we got fixed camera then we can render the ground once (kinda)
//use for exit hole/png and floor_ladder.png ?
//play some effect when you get a coin
//place light things around
//big white line on big screenes

/*

   maybe for the enemy if he collided with something on x direction he switch to fllowing on y?
effect that plays when you collect all coins
win screen (display move count? and be able to restart or exit)
improve movement ( test in map0 there is a weird thing whre it moves 2 squares
make particules independant of frame-rate
remove line_length and endian for all images except the window one
write all images to a file and read from it

slow down the game to see if we get kill correctly
removed last_keycode check if everything is ok
*/


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


typedef struct s_map {
	int		width;
	int		height;
	int		collectibles_count;
	char	**contents;//todo: store exit and start and collectibles positino
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

typedef struct s_particule {
	float x, y;
	float dx, dy;
	float size;
	float lifetime_left;
	float lifetime;
	float r, g, b;
}	t_particule;

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

typedef struct s_enemy {
	int frame;
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
	int follow_dir;
}t_enemy;

typedef struct s_light {
	int cx, cy;
	int radius;
	int r, g, b;
}t_light;


typedef struct s_game t_game;

struct s_game{
	int player_dead;
	float dead_t;

	void	*mlx;
	void	*window;

	t_map	original_map;
	t_map	map;
	int		cell_dim;
	int		moves_count;
	int		collected_count;

	int *floors;

	float light_scale;
	int window_scale;

	t_image back_ground;
	t_image window_image;
	t_image draw_image;
	t_image	light_image;


	t_image death_image;

	t_image wall_top;
	t_image wall_bottom;
	t_image wall_left;
	t_image wall_right;

	t_image floor_ladder;

	t_image floor[8];
	t_image door[2];
	t_image player_idle[4][2];
	t_image player_run[4][2];
	t_image enemy_idle[4][2];
	t_image enemy_run[4][2];

	int *place_torch;
	t_image torch[4];
	int torch_frame;

	int offset_x;
	int offset_y;

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

	t_particule particules[8192];
	int particule_count;
	
	char *images[64];
	int death_count;
};


void update_dir(t_game *game, float *visual_x, int dx, float *vel_x, int *game_x, int game_y, int is_x, float a, int is_player);
void draw_rect(t_image *image, int min_x, int min_y, int max_x, int max_y, unsigned int color);
void add_light_circle(t_game *game, int cx, int cy, int r, unsigned int color);
float dist_sq(float x0, float y0, float x1, float y1);
int exit_game(t_game *game, int failed);
int	parse_map(t_map *map, char *map_filename);
void draw_image(t_image *draw_image, t_image *image, int min_x, int min_y, int max_x, int max_y);
void init_game(t_game *game, char *map_file);
void update_and_draw_enemies(t_game *game);
void update_and_draw_player(t_game *game);

void emit_particules(t_game *game, t_particule_emitter *e);
void update_and_draw_particules(t_game *game);

unsigned int	lerp_color(unsigned color1, unsigned color2, float t);
#endif
