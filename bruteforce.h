#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include <stdint.h>

// Search [start, end) for the number whose SHA256 equals hash (32 bytes).
// Returns the number, or 0 if no match exists in the range.
uint64_t crack(const uint8_t *hash, uint64_t start, uint64_t end);

// Hash n's 8 bytes (little-endian, as the client does) into out.
void hash_u64(uint64_t n, uint8_t out[32]);

#endif
