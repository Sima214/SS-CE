#include "test_utils.h"

#include <Config.h>
#include <GAlloc.h>
#include <Macros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOCATION_SIZE 1024

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  #ifndef STANDALONE
    galloc_dump_stats();
    // Allocation test.
    int* leakage = malloc(ALLOCATION_SIZE);
    if(leakage == NULL) {
      puts("Could not allocate memory!");
      return EXIT_FAILURE;
    }
    void* min = malloc(1);
    printf("Minimum allocation is: %zu\n", galloc_size(min));
    memset(leakage, 0, ALLOCATION_SIZE);
    galloc_dump_stats();
    free(leakage);
  #else
    puts("Standalone build, test disabled!");
  #endif
  return EXIT_SUCCESS;
}