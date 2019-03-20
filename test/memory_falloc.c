#include "test_utils.h"

#include <FAlloc.h>
#include <Macros.h>

#include <stdint.h>
#include <stdio.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  int* p1 = falloc_malloc(3 * sizeof(int));
  char* c1 = falloc_malloc_aligned(sizeof(char), 1);
  int16_t* s1 = falloc_malloc_aligned(sizeof(int16_t), 2);
  int* p2 = falloc_malloc(16 * sizeof(int));
  fill_garbage(p2, 16 * sizeof(int));
  fill_garbage(p1, 3 * sizeof(int));
  *c1 = '\n';
  *s1 = -13;
  printf("%p\n%p\n%p\n%p\n", p1, c1, s1, p2);
  falloc_free(p2);
  falloc_free(s1);
  falloc_free(c1);
  falloc_free(p1);
  return EXIT_SUCCESS;
}