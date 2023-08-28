SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
RM = rm -f
NAME = so_long
CFLAGS = -Wall -Wextra
LDFLAGS = -lmlx -lXext -lX11 -lm #-lmlx #-framework OpenGL #-framework AppKit
LIBFT = libft/libft.a

all: $(NAME)

$(LIBFT): 
	make -C libft

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBFT) -o $(NAME)

clean: 
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
