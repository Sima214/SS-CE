#include "test_utils.h"

#include <FAlloc.h>
#include <HashSet.h>
#include <Macros.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define ADD_COUNT KBYTES(1)

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  srand(time(NULL));
  HashSet* hs = hashset_create(&IDT_INT, 0, -1, -1);
  if(hs == NULL) {
    return EXIT_FAILURE;
  }
  for(int v = 0; v < ADD_COUNT; v++) {
    if(hashset_add(hs, &v)) {
      return EXIT_FAILURE;
    }
    if(!hashset_contains(hs, &v)) {
      return EXIT_FAILURE;
    }
  }
  if(hash_size(hs) != ADD_COUNT) {
    return EXIT_FAILURE;
  }
  for(int v = 0; v < ADD_COUNT; v++) {
    if(hashset_remove(hs, &v)) {
      return EXIT_FAILURE;
    }
    if(hashset_contains(hs, &v)) {
      return EXIT_FAILURE;
    }
  }
  if(hash_size(hs) != 0) {
    return EXIT_FAILURE;
  }
  hashset_destroy(hs);
  return EXIT_SUCCESS;
}