#include "test_utils.h"

#include <Clock.h>
#include <Interface.h>
#include <Macros.h>
#include <Sort.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/*
 * Test results:
 *  Standard sort: 2700ns
 *  Heapsort v1: 3400ns
 *  Heapsort v2[def]: 4700ns
 *  Heapsort v2[custom]: 2600ns
 *  Heapsort v2[custom_memswap]: 3600ns
 */

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
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
  clock_reset(&pc);
  for(size_t i = 0; i < sizeof(test_area) / sizeof(int) / 64; i++) {
    int* array = test_area + (i * 64);
    __builtin_prefetch(array);
    clock_start(&pc);
    sort_heap(array, 64, &IDT_INT);
    clock_stop(&pc);
  }
  printf("method:\t AVG | MIN | MAX\n");
  printf("heapsort: %6.4f | %6.4f | %6.4f\n", pc.avg, pc.min, pc.max);
  for(size_t i = 0; i < sizeof(test_area) / sizeof(int) / 64; i++) {
    int* array = test_area + (i * 64);
    if(!is_sorted_i(array, 64)) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}