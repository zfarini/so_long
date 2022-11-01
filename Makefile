SRC = main.c
OBJ = $(SRC:.c=.o)
RM = rm -f
NAME = so_long
CFLAGS = -Wall -Wextra 
LDFLAGS = -lmlx -framework OpenGL -framework AppKit -fsanitize=address -fsanitize=undefined

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)

clean: 
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
