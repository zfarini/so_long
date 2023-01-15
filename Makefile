SRC = so_long.c map.c game_init.c enemies.c player.c particules.c movement.c background.c load_images.c game_restart.c draw_map.c draw.c light.c utils.c map_utils.c
OBJ = $(SRC:.c=.o)
RM = rm -f
NAME = so_long
CFLAGS = -Wall -Wextra  -fsanitize=address -fsanitize=undefined 
LDFLAGS =  -lmlx -framework OpenGL -framework AppKit -fsanitize=address -fsanitize=undefined

all: $(NAME)

$(NAME): $(OBJ)
	make -C libft
	$(CC) $(OBJ) $(LDFLAGS) libft/libft.a -o $(NAME)

clean: 
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
