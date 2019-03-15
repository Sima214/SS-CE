#include "test_utils.h"

#include <GAlloc.h>
#include <Macros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOCATION_SIZE 1024

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  malloc_stats_print(NULL, NULL, NULL);
  // Allocation test.
  int* leakage = malloc(ALLOCATION_SIZE);
  if(leakage == NULL) {
    puts("Could not allocate memory!");
    return EXIT_FAILURE;
  }
  memset(leakage, 0, ALLOCATION_SIZE);
  malloc_stats_print(NULL, NULL, NULL);
  free(leakage);
  return EXIT_SUCCESS;
}