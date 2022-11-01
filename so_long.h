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
	char	**contents;//todo: store exit and start and collectibles positino
}	t_map;

typedef struct s_game_state {
	void	*mlx;
	void	*window;
	int		width;
	int		height;
	void	*image;
	char	*pixels;
	int		bits_per_pixels;
	int		line_length;
	int		endian;
	t_map	map;
}	t_game_state;

#endif