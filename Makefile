SRC = so_long.c map.c game_init.c player.c background.c load_images.c game_restart.c draw_map.c draw.c utils.c map_utils.c
BSRC = so_long.c map.c game_init.c enemies.c player.c particules.c movement.c background.c load_images.c game_restart.c draw_map.c draw.c light.c utils.c map_utils.c
OBJS = $(SRC:.c=.o)
BOBJ = $(BSRC:.c=.o)
BOBJS = $(addprefix bonus/, ${BOBJ})
RM = rm -f
NAME = so_long
CFLAGS = -Wall -Wextra -Werror
LDFLAGS =  -lmlx -framework OpenGL -framework AppKit
LIBFT = libft/libft.a

all: $(NAME)

$(LIBFT): 
	make -C libft

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBFT) -o $(NAME)

bonus: $(BOBJS) $(LIBFT)
	$(CC) $(BOBJS) $(LDFLAGS) $(LIBFT) -o $(NAME)

clean: 
	$(RM) $(OBJS) $(BOBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re
