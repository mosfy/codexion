NAME = codexion
OBJDIR = obj
CC = cc
RM = rm -f
CFLAGS = -Wall -Wextra -Werror -g3 -pthread

SRC 		= main.c\
			  init1.c\
			  time.c\
			  coder.c\
			  simulation.c\
			  heap.c\
			  src/ft_isdigit.c\
			  src/ft_atoi.c\
			  src/ft_calloc.c\
			  src/ft_bzero.c\

OBJ = $(SRC:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(LIBFT) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -lpthread

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY:		all clean fclean re