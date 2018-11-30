#include "test_utils.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <Clock.h>
#include <Interface.h>
#include <Macros.h>
#include <Sort.h>
#include <Strings.h>

/*
 * Test results:
 *  Standard sort: 2700ns
 *  Heapsort v1: 3400ns
 *  Heapsort v2[def]: 4700ns
 *  Heapsort v2[custom]: 2600ns
 *  Heapsort v2[custom_memswap]: 3600ns
 */

/*
 * Test if array of integers is ascending order.
 */
static int is_sorted_i(int* a, size_t n) {
  for(size_t i = 1; i < n; i++) {
    if(!(a[i] >= a[i - 1])) {
      return 0;
    }
  }
  return 1;
}

int cst_cmp_e(MARK_UNUSED const IDataType* ignored, int* a, int* b) {
  return a == b;
}

int cst_cmp_l(MARK_UNUSED const IDataType* ignored, int* a, int* b) {
  return a < b;
}

int cst_cmp_le(MARK_UNUSED const IDataType* ignored, int* a, int* b) {
  return a <= b;
}

void cst_swap(MARK_UNUSED const IDataType* ignored, int* a, int* b) {
  //memswap(a, b, 4);
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
const IDataType DTI_CST = {4, 0, 4, INTERFACE_TYPE_CUSTOM,
                           (Compare)cst_cmp_e, (Compare)cst_cmp_l,
                           (Compare)cst_cmp_le, (Operate)cst_swap};
const IDataType DTI_DEF = {4, 0, 4, 0, NULL, NULL, NULL, NULL};
const IDataType DTI_ODD = {4, 0, 3, 0, NULL, NULL, NULL, NULL};

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
  ssce_reset(&pc);
  for(size_t i = 0; i < sizeof(test_area) / sizeof(int) / 64; i++) {
    int* array = test_area + (i * 64);
    __builtin_prefetch(array);
    ssce_start(&pc);
    heapsort(array, 64, &DTI_DEF);
    ssce_stop(&pc);
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