#include "test_utils.h"

#include <Heap.h>
#include <Macros.h>

#include <string.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  // Random input used for testing.
  static int a[] = {2, 7, 36, 27, 29, 27, 22, 31, 32, 12, 8, 23};
  static const int h[] = {36, 32, 27, 31, 29, 23, 22, 7, 27, 12, 8, 2};
  static const size_t n = sizeof(a) / sizeof(int);
  // Test heap creation.
  heap_create(a, n, &IDT_INT);
  if(memcmp(a, h, n)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}