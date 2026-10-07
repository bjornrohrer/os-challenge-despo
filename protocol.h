#ifndef OS_CHALLENGE_DESPO_PROTOCOL_H
#define OS_CHALLENGE_DESPO_PROTOCOL_H
#include <stdint.h>
#include <string.h>
#include "messages.h"

void *handle_client(void *arg);

struct request {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    uint64_t start;
    uint64_t end;
    uint8_t priority;
    int client_fd;
};
#endif

