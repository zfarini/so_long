SRC = so_long.c map.c
OBJ = $(SRC:.c=.o)
RM = rm -f
NAME = so_long
CFLAGS = -Wall -Wextra# -fsanitize=address -fsanitize=undefined 
LDFLAGS =  -framework OpenGL -framework AppKit #-fsanitize=address -fsanitize=undefined

all: $(NAME)

$(NAME): $(OBJ)
	make -C libft
	$(CC) $(OBJ) $(LDFLAGS) libft/libft.a mlx/libmlx.a -o $(NAME)

clean: 
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
