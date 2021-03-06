#ifndef TEST_UTILS_H
#define TEST_UTILS_H
/**
 * Common code for all tests.
 */

#include <Interface.h>
#include <Macros.h>

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
 * Flips random bits in an array.
 */
static inline void fill_light_garbage(void* array, size_t bytes) {
  char* q = array;
  char* const end_bytes = q + bytes;
  for(/* NOP */; q < end_bytes; q++) {
    char r = rand() % (sizeof(size_t) * __CHAR_BIT__ + 1);
    // r -> [0, 64]
    if(r) {
      // r -> [0, 63]
      r--;
      MASK_SET(*q, MASK_CREATE(r), 1);
    }
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

static int cst_cmp_e(MARK_UNUSED const IDataType* ignored, const int* a, const int* b) {
  return *a == *b;
}

static int cst_cmp_l(MARK_UNUSED const IDataType* ignored, const int* a, const int* b) {
  return *a < *b;
}

static int cst_cmp_le(MARK_UNUSED const IDataType* ignored, const int* a, const int* b) {
  return *a <= *b;
}

static void cst_swap(MARK_UNUSED const IDataType* ignored, int* a, int* b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

static size_t cst_hash(MARK_UNUSED const IDataType* ignored, const int* k) {
  return *k;
}

const IDataType IDT_INT = {4, 0, 4, (Compare)cst_cmp_e, (Compare)cst_cmp_l, (Compare)cst_cmp_le, (Operate)cst_swap, (Calculate)cst_hash};

#endif /*TEST_UTILS_H*/