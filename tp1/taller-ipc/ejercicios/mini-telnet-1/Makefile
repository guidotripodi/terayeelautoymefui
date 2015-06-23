.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall -Werror

all: server client

server:
	$(CC) $(CFLAGS) -o mt_server mt_server.c

client:
	$(CC) $(CFLAGS) -o mt_client mt_client.c

clean:
	rm -f mt_server mt_client