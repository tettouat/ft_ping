# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/06/07 17:57:17 by tettouat          #+#    #+#              #
#    Updated: 2017/06/08 15:35:50 by tettouat         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ft_ping

FLAGS		=	-std=gnu99 -lrt -Wall -Wextra -Werror

SRCDIR		=	srcs/

INCDIR		=	includes/

OBJDIR		=	objs/

SRC			=	main.c \
				ping.c \
				get_addr.c

SRCS =	$(addprefix $(SRCDIR), $(SRC))

OBJ	= $(addprefix $(OBJDIR), $(SRC:.c=.o))

all:		$(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@mkdir -p $(dir $(OBJ))

$(OBJDIR)%.o : $(SRCDIR)%.c | $(OBJDIR)
	@gcc $(FLAGS) -c $< -o $@ -I $(INCDIR)

$(NAME):	$(OBJDIR) $(OBJ)
	@gcc $(FLAGS) -o $(NAME) $(OBJ)

clean:
	if [ -d $(OBJDIR) ]; then rm -r $(OBJDIR); fi

fclean:	clean
	@rm -f $(NAME)

re:	fclean all


.PHONY: fclean clean re 

.SILENT: clean
