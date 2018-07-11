#include "test_utils.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <structures/Interface.h>
#include <structures/Sort.h>
#include <clock/Clock.h>

/*
 * Standard sort: 2700ns
 * Heapsort v1: 3400ns
 */

/*
 * Test if array of integers is ascending order.
 */
static int is_sorted_i(int* a, size_t n) {
  for(size_t i=1; i<n; i++) {
    if(!(a[i] >= a[i-1])) {
      return 0;
    }
  }
  return 1;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  // Source data structure.
  static int garbage[MBYTES(4)];
  static int test_area[MBYTES(4)];
  fill_garbage(garbage, sizeof(garbage));
  memcpy(test_area, garbage, sizeof(test_area));
  /*
   * Split arrays to 64 chunks.
   */
  PerfClock pc;
  ssce_reset(&pc);
  for(size_t i=0; i<sizeof(test_area)/sizeof(int)/64; i++) {
    int* array = test_area + (i*64);
    __builtin_prefetch(array);
    ssce_start(&pc);
    //qsort(array, 64, 4, int_cmp_gen);
    //heapsort(array, 64, &iinterface);
    ssce_stop(&pc);
  }
  printf("method:\t AVG | MIN | MAX\n");
  printf("heapsort: %6.4f | %6.4f | %6.4f\n", pc.avg, pc.min, pc.max);
  for(size_t i=0; i<sizeof(test_area)/sizeof(int)/64; i++) {
    int* array = test_area + (i*64);
    if(!is_sorted_i(array, 64)) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}