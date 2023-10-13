# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ccottin <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/13 19:38:32 by ccottin           #+#    #+#              #
#    Updated: 2023/10/13 20:58:59 by ccottin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ft_otp

SRCS	= main.cpp otop.cpp

CC	= c++

OBJS	= $(SRCS:.cpp=.o)

DEPS	= $(OBJS:.o=.d)

OPSSLFLAGS = $(pkg-config --cflags --libs openssl)

CFLAGS	= -Wall -Wextra -Werror -MMD -g3

all	:	$(NAME)

%.o	:	%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME)	:	$(OBJS)
	$(CC) $(CFLAGS) $(OPSSLFLAGS) $(OBJS) -o $@

clean	:
	rm -rf $(OBJS)
	rm -rf $(DEPS)

fclean	:	clean
	rm -rf $(NAME)

re	:	fclean
	make

-include = $(DEPS)

.PHONY	:	all fclean clean re
