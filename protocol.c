#include "protocol.h"
#include <stdint.h>
#include <string.h>
#include "messages.h"

void parse_request(
    const uint8_t packet[49],uint8_t hash[32], uint64_t *start, uint64_t *end,uint8_t *priority)
    {
    uint64_t start_network;
    uint64_t end_network;

    memcpy(hash,packet,32);
    memcpy(&start_network,packet+32,8);
    memcpy(&end_network,packet+40,8);

    *start = be64toh(start_network);
    *end = be64toh(end_network);
    *priority = packet[48];
}