#include "test_utils.h"

#include <Macros.h>
#include <SortedArray.h>

#include <string.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  SortedArray* sai = sorted_array_create(&IDT_INT);
  if(sai == NULL || sorted_array_size(sai) != 0) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}