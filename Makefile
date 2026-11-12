.DEFAULT_GOAL := all

STROS = Windows
OS := $(shell uname)
ifeq ($(OS), Darwin)
	STROS := darwin
endif 
ifeq ($(OS), Linux)
	STROS := linux
else
	STROS := unknown
endif

NAME = exposx
CC = cc
CFLAGS = -g -Wall -Wextra -Werror
SRC = cli.c \

check_os:
	@if [ "$(STROS)" = "unknown" ]; then \
		echo 'Unsuported OS Detected: "$(STROS)"'; exit 1; \
	else \
		echo "Detected OS: $(STROS)";\
	fi

check_package:
	@if dpkg -s libcurl4-openssl-dev > /dev/null 2>&1; then\
		echo "Libcurl is already installed";\
	else\
		echo "Libcurl is not installed";\
		echo "Download it..";\
		sudo apt-get install -y libcurl4-openssl-dev;\
	fi
	@if dpkg -s clang > /dev/null 2>&1; then\
		echo "Clang is already installed";\
	else\
		echo "Clang is not installed";\
		echo "Download it..";\
		sudo apt-get install -y clang;\
	fi
	@if dpkg -s libarchive-dev > /dev/null 2>&1; then\
		echo "Libarchive is already installed";\
	else\
		echo "Libarchive is not installed";\
		echo "Download it..";\
		sudo apt-get install -y libarchive-dev;\
	fi

NAME := exposx_$(STROS)

OBJS = $(SRC:.c=.o)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lcurl -larchive

all: check_os check_package $(NAME)

clean: 
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all check_package check_os clean fclean re
