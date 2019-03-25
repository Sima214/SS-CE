#include "test_utils.h"

#include <Macros.h>
#include <SortedArray.h>

#include <stdio.h>
#include <string.h>

static int stress() {
  return EXIT_SUCCESS;
}

static inline void dump_sorted_array(SortedArray* sa) {
  size_t len = sorted_array_size(sa);
  for(size_t i = 0; i < len; i++) {
    int value = 0;
    sorted_array_get(sa, &value, i);
    if(i == 0) {
      printf("{");
    }
    printf("%d", value);
    if(i < len - 1) {
      printf(", ");
    }
    else {
      printf("}");
    }
  }
  puts("");
}

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  if(argc > 1) {
    return stress();
  }
  const int TEST_NUMS[] = {-33, 35, 25, -22, 25, 25, 20, -9, 33, 30, -8, -18};
  const int insert_stable_const = 0;
  const int insert_const = 122;
  SortedArray* sai = sorted_array_create(&IDT_INT);
  if(sai == NULL || sorted_array_size(sai) != 0) {
    return EXIT_FAILURE;
  }
  sorted_array_merge(sai, TEST_NUMS, sizeof(TEST_NUMS) / sizeof(int));
  dump_sorted_array(sai);
  sorted_array_delete_stable(sai, &TEST_NUMS[8]);
  sorted_array_insert_stable(sai, &insert_stable_const);
  sorted_array_delete(sai, &TEST_NUMS[1]);
  sorted_array_insert(sai, &insert_const);
  dump_sorted_array(sai);
  sorted_array_destroy(sai);
  return EXIT_SUCCESS;
}