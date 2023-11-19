CC = gcc
CFLAGS = -O2 -std=c99
SDL_FLAGS = -lSDL2

TARGET = main
NAME = mpt

SRC = main.c IoHandler.c MapTrace.c
HED = IoHandler.h MapTrace.h

SRC_DIR = ./src

$(TARGET): $(addprefix $(SRC_DIR)/, $(HED))
	$(CC) $(CFLAGS) $(SDL_FLAGS) $(addprefix $(SRC_DIR)/, $(SRC)) -o $(NAME)

clean:
	rm -f $(NAME)