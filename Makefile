CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -pthread

server: server.c bruteforce.c messages.h lonesha256.h bruteforce.h
	$(CC) $(CFLAGS) -o server server.c bruteforce.c

test-bruteforce: test-bruteforce.c bruteforce.c bruteforce.h lonesha256.h
	$(CC) $(CFLAGS) -o test-bruteforce test-bruteforce.c bruteforce.c

.PHONY: test clean
test: test-bruteforce
	./test-bruteforce

clean:
	rm -f server test-bruteforce
