CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -pthread -I/opt/homebrew/opt/openssl@3/include

all: server

server: server.c bruteforce.c protocol.c protocol.h messages.h lonesha256.h bruteforce.h
	$(CC) $(CFLAGS) -o server server.c bruteforce.c protocol.c

test-bruteforce: test-bruteforce.c bruteforce.c bruteforce.h lonesha256.h
	$(CC) $(CFLAGS) -o test-bruteforce test-bruteforce.c bruteforce.c

.PHONY: all test clean
test: test-bruteforce
	./test-bruteforce

clean:
	rm -f server test-bruteforce
