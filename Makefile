ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINKNAME = libft_malloc.so
SRCDIR = srcs/
INCDIR = includes/
SRCFILES = malloc.c\
		   free.c\
		   realloc.c\
		   utils.c\
		   print.c\

INCFILES = malloc.h
SRC = $(addprefix $(SRCDIR),$(SRCFILES))
INC = $(addprefix $(INCDIR),$(INCFILES))
OBJ = $(SRC:.c=.o)
FLAG = -I includes # -Wall -Werror -Wextra 

all: $(NAME)

%.o: %.c $(INC)
	gcc -c -fPIC -o $@ $< $(FLAG)

$(NAME):$(OBJ) $(INC)
	gcc -shared -o $@ $(OBJ) $(FLAG)
	ln -s $(NAME) $(LINKNAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(LINKNAME)

re: fclean all