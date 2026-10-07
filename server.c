#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "bruteforce.h"
#include <stdint.h>
#include <pthread.h>
#include "messages.h"
#include "protocol.h"



int main(int argc, char *argv[]) {

  // check how many inputs, has to be two
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    return 1;
  }

  // convert argv[1] to a long
  char *end;
  long port = strtol(argv[1], &end, 10);
  if (*end != '\0' || port < 1 || port > 65535) {
    fprintf(stderr, "invalid port: %s\n", argv[1]);
    return 1;
  }

  // ignore SIGPIPE so a dead client can't kill the server
  signal(SIGPIPE, SIG_IGN);

  // initialize the socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    return 1;
  }

  // socket can be restarted immediatly
  int yes = 1;
  int setsockop_fd = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (setsockop_fd == -1) {
    perror("setsockopt");
    return 1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  int bind_fd = bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
  if (bind_fd == -1) {
    perror("bind");
    return 1;
  }

  int listen_fd = listen(server_fd, 8);
  if (listen_fd == -1) {
    perror("listen");
    return 1;
  }

  printf("server listening on port %s\n", argv[1]);
    // TODO: multiple clients on same server at same time
  for (;;) {
    // wait here until a client connects; get a NEW fd for that client
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
      perror("accept");
      continue; // this client failed; keep serving others
    }

    // allow multiple clients to access the server at the same time
    pthread_t tid;
    int rc = pthread_create(&tid, NULL, handle_client,(void *)(intptr_t) client_fd);

    if (rc != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(rc));
        close(client_fd);
        continue;
    }
    pthread_detach(tid);
  }
}
