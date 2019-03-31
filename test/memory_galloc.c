#include "test_utils.h"

#include <GAlloc.h>
#include <Macros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOCATION_SIZE 1024

#define MALLOC_STATS_PRINT_OPTS "gamble"

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  #ifndef VALGRIND
    malloc_stats_print(NULL, NULL, MALLOC_STATS_PRINT_OPTS);
    // Allocation test.
    int* leakage = malloc(ALLOCATION_SIZE);
    if(leakage == NULL) {
      puts("Could not allocate memory!");
      return EXIT_FAILURE;
    }
    void* min = malloc(1);
    printf("Minimum allocation is: %u\n", malloc_usable_size(min));
    memset(leakage, 0, ALLOCATION_SIZE);
    malloc_stats_print(NULL, NULL, MALLOC_STATS_PRINT_OPTS);
    free(leakage);
    return EXIT_SUCCESS;
  #else
    return EXIT_FAILURE;
  #endif
}