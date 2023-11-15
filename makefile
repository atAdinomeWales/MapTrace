CC = gcc
CFLAGS = -O2 -std=c99
SDL_FLAGS = -lSDL2

TARGET = main
NAME = mpt

$(TARGET):
	$(CC) $(CFLAGS) $(SDL_FLAGS) main.c -o $(NAME)

clean:
	rm -f $(NAME)