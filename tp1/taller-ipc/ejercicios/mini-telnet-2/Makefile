.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall -Werror

all: server client

server:
	$(CC) $(CFLAGS) -o mt_server2 mt_server2.c

client:
	$(CC) $(CFLAGS) -o mt_client2 mt_client2.c

clean:
	rm -f mt_server2 mt_client2