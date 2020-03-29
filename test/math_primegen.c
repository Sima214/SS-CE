#include "test_utils.h"

#include <Macros.h>
#include <PrimeGenerator.h>

#include <stdlib.h>
#include <stdio.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  prime_t prime = primegen_new();
  while (1) {
    prime = primegen_get_next(prime);
    printf("%zu -> %ju\n", prime.last_prime_index, prime.last_prime);
  }
  
  return EXIT_SUCCESS;
}