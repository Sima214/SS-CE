#include "test_utils.h"

#include <Hash.h>
#include <Macros.h>

#include <stdint.h>
#include <stdlib.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  uint64_t t1 = 0;
  uint32_t r1 = 3736311059;

  if(ncrypto_xxhash32(&t1, sizeof(t1), 0) != r1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}