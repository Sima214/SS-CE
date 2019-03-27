#include "test_utils.h"

#include <Macros.h>
#include <SortedArray.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STRESS_SECTIONS_LENGTH 4096 / sizeof(int)
#define STRESS_SECTIONS_COUNT 128

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

static int stress() {
  static int RNG[STRESS_SECTIONS_LENGTH * STRESS_SECTIONS_COUNT];
  time_t seed = time(NULL);
  printf("Rng seed: %lu\n", seed);
  srand(seed);
  fill_garbage(RNG, sizeof(RNG));
  SortedArray* sai = sorted_array_create(&IDT_INT);
  if(sai == NULL) {
    return EXIT_FAILURE;
  }
  for(size_t i = 0; i < STRESS_SECTIONS_COUNT; i++) {
    int* base = RNG + STRESS_SECTIONS_LENGTH * i;
    printf("Calculating [%u/%u]\n", (uint32_t)i, (uint32_t)STRESS_SECTIONS_COUNT);
    // Insert or rarely merge
    if((rand() % 128) == 0) {
      puts("Merging current section!");
      size_t previous_size = sorted_array_size(sai);
      sorted_array_merge(sai, base, STRESS_SECTIONS_LENGTH);
      size_t after_size = sorted_array_size(sai);
      if((previous_size + STRESS_SECTIONS_LENGTH) != after_size) {
        puts("Incorrect size!");
      }
    }
    else {
      for(size_t j = 0; j < STRESS_SECTIONS_LENGTH; j++) {
        int* current = base + j;
        printf("Inserting %i\n", *current);
        size_t previous_size = sorted_array_size(sai);
        if(sorted_array_insert(sai, current) == INVALID_SIZE_T) {
          return EXIT_FAILURE;
        }
        size_t after_size = sorted_array_size(sai);
        if((previous_size + 1) != after_size) {
          puts("Incorrect size!");
        }
      }
    }
    // Erase
    for(int l = 0; l < (int)(STRESS_SECTIONS_LENGTH / 128); l++) {
      size_t previous_size = sorted_array_size(sai);
      size_t index_to_erase = rand() % previous_size;
      printf("Deleting index: %u\n", (uint32_t)index_to_erase);
      if(sorted_array_erase(sai, index_to_erase)) {
        return EXIT_FAILURE;
      }
      size_t after_size = sorted_array_size(sai);
      if((previous_size - 1) != after_size) {
        puts("Incorrect size!");
      }
    }
    // Optimize
    sorted_array_compact(sai);
    if(!is_sorted_i(sorted_array_pointer(sai), sorted_array_size(sai))) {
      puts("Array is not of correct order!");
      return EXIT_FAILURE;
    }
  }
  // Cleanup
  sorted_array_destroy(sai);
  return EXIT_SUCCESS;
}

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  if(argc > 1) {
    return stress();
  }
  const int TEST_NUMS[] = {-33, 35, 25, -22, 25, 25, 20, -9, 33, 30, -8, -18};
  const int TEST_NUMS2[] = {18, 7, 13, -2};
  const int insert_stable_const = 0;
  const int insert_const = 122;
  SortedArray* sai = sorted_array_create(&IDT_INT);
  if(sai == NULL || sorted_array_size(sai) != 0) {
    return EXIT_FAILURE;
  }
  if(sorted_array_merge(sai, TEST_NUMS, sizeof(TEST_NUMS) / sizeof(int))) {
    return EXIT_FAILURE;
  }
  dump_sorted_array(sai);
  if(!is_sorted_i(sorted_array_pointer(sai), sorted_array_size(sai))) {
    return EXIT_FAILURE;
  }
  if(sorted_array_delete_stable(sai, &TEST_NUMS[8]) == INVALID_SIZE_T) {
    return EXIT_FAILURE;
  }
  if(sorted_array_insert_stable(sai, &insert_stable_const) == INVALID_SIZE_T) {
    return EXIT_FAILURE;
  }
  if(sorted_array_delete(sai, &TEST_NUMS[1]) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  if(sorted_array_insert(sai, &insert_const) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  dump_sorted_array(sai);
  if(!is_sorted_i(sorted_array_pointer(sai), sorted_array_size(sai))) {
    return EXIT_FAILURE;
  }
  if(sorted_array_merge(sai, TEST_NUMS2, sizeof(TEST_NUMS2) / sizeof(int))) {
    return EXIT_FAILURE;
  }
  dump_sorted_array(sai);
  if(!is_sorted_i(sorted_array_pointer(sai), sorted_array_size(sai))) {
    return EXIT_FAILURE;
  }
  sorted_array_destroy(sai);
  return EXIT_SUCCESS;
}