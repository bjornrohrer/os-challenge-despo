#define LONESHA256_IMPLEMENTATION
#define LONESHA256_STATIC
#include "bruteforce.h"
#include "lonesha256.h"
#include <stdint.h>
#include <string.h>

// Function that compares wether the hash has the same amount of bytes as the hash we create
uint64_t crack(const uint8_t *hash, uint64_t start, uint64_t end) {
    uint8_t buff[32];

    while (start < end) {
        hash_u64(start, buff);

        if (memcmp(hash, buff, 32) == 0) {
            return start;
        }
        start++;
    }

    return 0;

}

void hash_u64(uint64_t n, uint8_t out[32]) {
    uint8_t in[8]; // 8 bytes = the 64 bits of n

    // Split n into its 8 bytes, lowest byte first (little-endian).
    for (int i = 0; i < 8; i++) {
        in[i] = (uint8_t)(n >> (8 * i));
    }

    // Hash those 8 bytes; the 32-byte result is written into out.
    lonesha256(out, in, 8);
}
