
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS=-lpthread
BIN=fork
SOURCE=fork.c

all:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)

run: all
	./$(BIN)

clean:
	rm $(BIN)
