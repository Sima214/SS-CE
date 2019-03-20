#include "test_utils.h"

#include <FAlloc.h>
#include <Macros.h>

#include <stdint.h>
#include <stdio.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  falloc_ptr(int) p1 = falloc_malloc(3 * sizeof(int));
  falloc_ptr(char) c1 = falloc_malloc_aligned(sizeof(char), 1);
  falloc_ptr(int16_t) s1 = falloc_malloc_aligned(sizeof(int16_t), 2);
  falloc_ptr(int) p2 = falloc_malloc(16 * sizeof(int));
  fill_garbage(p2, 16 * sizeof(int));
  fill_garbage(p1, 3 * sizeof(int));
  *c1 = '\n';
  *s1 = -13;
  printf("%p\n%p\n%p\n%p\n", p1, c1, s1, p2);
  return EXIT_SUCCESS;
}