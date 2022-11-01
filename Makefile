SRC = so_long.c map.c
OBJ = $(SRC:.c=.o)
RM = rm -f
NAME = so_long
CFLAGS = -Wall -Wextra
LDFLAGS = -lmlx -framework OpenGL -framework AppKit -fsanitize=address -fsanitize=undefined

all: $(NAME)

$(NAME): $(OBJ)
	make -C libft
	make -C get_next_line
	$(CC) $(OBJ) $(LDFLAGS) libft/libft.a get_next_line/libgnl.a -o $(NAME)

clean: 
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
