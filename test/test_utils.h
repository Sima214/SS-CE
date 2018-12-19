#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stddef.h>
#include <stdlib.h>

/*
 * Helper for tests.
 */
#define KBYTES(kb) kb * 1024
#define MBYTES(mb) KBYTES(mb * 1024)

/*
 * Fills an array with random data.
 */
static inline void fill_garbage(void* array, size_t bytes) {
  size_t words = bytes / 4;
  bytes = bytes % 4;
  int* p;
  int* const end_words = ((int*)array) + words;
  char* const end_bytes = ((char*)end_words) + bytes;
  for(p = (int*)array; p < end_words; p++) {
    int r = rand();
    *p = r;
  }
  char* q;
  for(q = (char*)p; q < end_bytes; q++) {
    char r = rand() % 256;
    *q = r;
  }
}

/*
 * Test if array of integers is in ascending order.
 */
static inline int is_sorted_i(int* a, size_t n) {
  for(size_t i = 1; i < n; i++) {
    if(!(a[i] >= a[i - 1])) {
      return 0;
    }
  }
  return 1;
}
#endif /*TEST_UTILS_H*/