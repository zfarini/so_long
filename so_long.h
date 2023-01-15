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

test error messages
torch positions (check map0)
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
	ESCAPE = 53,
	KEY_UP = 13,
	KEY_DOWN = 1,
	KEY_LEFT = 0,
	KEY_RIGHT = 2,
};
#endif

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

typedef struct s_move {
	float *visual_p[2];
	int dx[2];
	float *vel[2];
	int *game_p[2];
	float a[2];
	float delta[2];	
	int is_player;
	int target;
	int dir;
} t_move;


typedef struct s_map {
	int		width;
	int		height;
	int		collectibles_count;
	char	**arr;//todo: store exit and start and collectibles positino
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
	int				cx;
	int				cy;
	int				r;
	unsigned int	color;
}	t_light;


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

	t_image background;
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
	int 	player_dx;
	int 	player_dy;
	float 	player_vel_x;
	float 	player_vel_y;

	t_image coin[4];
	int coin_frame; // multiple coins!
	
	int enemies_count;
	t_enemy *enemies;

	t_particule particules[8192];
	int particule_count;
	
	char *images[64];
	int death_count;

	int data_read_fd;
};

void	*ft_alloc(t_game *game, size_t size);

void	draw_map(t_game *game);

void	init_background(t_game *game);
void exit_game(t_game *game, int failed);

void	restart_game(t_game *game);

void	do_move(t_game *game, t_move move);


void draw_particule(t_game *game, t_particule *p, unsigned int color);


void add_light_circle(t_game *game, t_light light);
int	parse_map(t_map *map, char *map_filename);

void	draw_image(t_game *game, t_image *image, int min_x, int min_y);
void init_game(t_game *game, char *map_file);
void update_and_draw_enemies(t_game *game);
void update_and_draw_player(t_game *game);

void emit_particules(t_game *game, t_particule_emitter *e);
void update_and_draw_particules(t_game *game);

void	load_all_images(t_game *game);

void	add_light_and_draw_to_window_image(t_game *game);

unsigned int	lerp_color(unsigned color1, unsigned color2, float t);


float clamp(float x, float min, float max);

void draw_death_screen(t_game *game);
#endif
