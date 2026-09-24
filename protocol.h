#ifndef OS_CHALLENGE_DESPO_PROTOCOL_H
#define OS_CHALLENGE_DESPO_PROTOCOL_H
#include <stdint.h>
#include <string.h>
#include "messages.h"

void parse_request(
    const uint8_t packet[49],uint8_t hash[32], uint64_t *start, uint64_t *end,uint8_t *priority);
#endif // OS_CHALLENGE_DESPO_PROTOCOL_H
