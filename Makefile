CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c17

listener: listener.c
	$(CC) $(CFLAGS) listener.c -g -o listener

client: client.c
	$(CC) $(CFLAGS) client.c -g -o client

clean:
	rm run 