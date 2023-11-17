CC = gcc
CFLAGS = -O2 -std=c99
SDL_FLAGS = -lSDL2

TARGET = main
NAME = mpt

SRC = main.c IoHandler.c MapTrace.c
HED = IoHandler.h MapTrace.h

$(TARGET): $(HED)
	$(CC) $(CFLAGS) $(SDL_FLAGS) $(SRC) -o $(NAME)

clean:
	rm -f $(NAME)