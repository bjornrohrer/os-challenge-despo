CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -pthread

server: server.c messages.h lonesha256.h
	$(CC) $(CFLAGS) -o server server.c

.PHONY: all clean

all: server

clean:
	rm -f server
