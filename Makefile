CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c17
all: main

main: main.c
	$(CC) $(CFLAGS) main.c -g -o run

clean:
	rm run 