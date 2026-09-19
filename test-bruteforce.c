#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "bruteforce.h"

int main(void) {
    uint8_t buff[32];
    hash_u64(42, buff);

   assert(crack(buff, 0, 100) == 42);
   printf("%s\n", "test 1 OK");

   hash_u64(5, buff);
   assert(crack(buff, 5, 100) == 5);
   printf("%s\n", "test 2 OK");

   hash_u64(99, buff);
   assert(crack(buff, 0, 100) == 99);
   printf("%s\n", "test 3 OK");
}
