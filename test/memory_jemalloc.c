#include "test_utils.h"

#include <jemalloc.h>
#include <Macros.h>

#include <stdlib.h>

void leak(size_t c) {
  // Leak some memory.
  volatile int* leakage = malloc(c * 32);
  for(size_t i = 0; i < c; i++) {
    leakage[i * 16] = leakage[i * 16 + 1];
  }
}

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  je_malloc_stats_print(NULL, NULL, NULL);
  leak(1024);
  je_malloc_stats_print(NULL, NULL, NULL);
  return EXIT_SUCCESS;
}