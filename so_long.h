#ifndef SO_LONG_H
#define SO_LONG_H
# include <mlx.h>
# include <assert.h>
# include <stdio.h>
# include "libft/libft.h"
# include "get_next_line/get_next_line.h"

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
	int		bits_per_pixels;
	char	*pixels;
	void	*img;
}	t_image;
typedef struct s_game_state {
	void	*mlx;
	void	*window;
	int		width;
	int		height;
	t_image draw_image;
	int		cell_width;
	int		cell_height;
	int		player_row;
	int		player_col;
	t_map	map;
	int		moves_count;
	int		collected_count;
	t_image player_image;
}	t_game_state;



int	parse_map(t_map *map, char *map_filename);
#endif