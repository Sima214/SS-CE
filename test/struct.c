#include <stdio.h>

#include <structures/Interface.h>
#include <structures/Sort.h>
#include <clock/Clock.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Standard sort: 2700ns
 * Heapsort v1: 3400ns
 */

#define KBYTES(kb) kb*1024
#define MBYTES(mb) KBYTES(mb*1024)

static int int_cmp_eq(const int* data, size_t i, size_t j) {
  return data[i] == data[j];
}

static int int_cmp_l(const int* data, size_t i, size_t j) {
  return data[i] < data[j];
}

static int int_cmp_le(const int* data, size_t i, size_t j) {
  return data[i] <= data[j];
}

static void int_swap(int* data, size_t i, size_t j) {
  int tmp = data[i];
  data[i] = data[j];
  data[j] = tmp;
}

static int int_cmp_gen(int* a, int* b) {
  return *a - *b;
}

const DataTypeInterface iinterface = {sizeof(char), (Compare) int_cmp_eq,
  (Compare) int_cmp_l, (Compare) int_cmp_le, (Operate) int_swap};

/*
 * Fills an array with random data.
 */
static void fill_garbage(void* array, size_t bytes) {
  size_t words = bytes/4;
  bytes = bytes%4;
  int* p;
  int* const end_words = ((int*)array) + words;
  char* const end_bytes = ((char*)end_words) + bytes;
  for(p=array; p<end_words; p++) {
    int r = rand();
    *p = r;
  }
  char* q;
  for(q=(char*)p; q<end_bytes; q++) {
    char r = rand()%256;
    *q = r;
  }
}

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
    heapsort(array, 64, &iinterface);
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