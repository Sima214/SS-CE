#include "test_utils.h"

#include <Dequeue.h>
#include <Macros.h>

#include <stdint.h>
#include <stdio.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  const int TEST_NUMS[] = {1, 2, 3, 4};
  Dequeue* dq = dequeue_create(&IDT_INT);
  for(size_t l = 1; l <= (sizeof(TEST_NUMS) / sizeof(int)); l++) {
    printf("Performing %u length test...\n", (uint32_t)l);
    // Test push front, pop front.
    puts("(f, f)");
    for(size_t i = 0; i < l; i++) {
      dequeue_push_front(dq, &TEST_NUMS[i]);
    }
    for(size_t i = 0; i < l; i++) {
      int result = 0;
      dequeue_pop_front(dq, &result);
      if(result != TEST_NUMS[l - i - 1]) {
        return EXIT_FAILURE;
      }
    }
    if(dequeue_size(dq) != 0) {
      return EXIT_FAILURE;
    }
    dequeue_reset(dq);
    // Test push front, pop back.
    puts("(f, b)");
    for(size_t i = 0; i < l; i++) {
      dequeue_push_front(dq, &TEST_NUMS[i]);
    }
    for(size_t i = 0; i < l; i++) {
      int result = 0;
      dequeue_pop_back(dq, &result);
      if(result != TEST_NUMS[i]) {
        return EXIT_FAILURE;
      }
    }
    if(dequeue_size(dq) != 0) {
      return EXIT_FAILURE;
    }
    dequeue_reset(dq);
    // Test push back, pop front.
    puts("(b, f)");
    for(size_t i = 0; i < l; i++) {
      dequeue_push_back(dq, &TEST_NUMS[i]);
    }
    for(size_t i = 0; i < l; i++) {
      int result = 0;
      dequeue_pop_front(dq, &result);
      if(result != TEST_NUMS[i]) {
        return EXIT_FAILURE;
      }
    }
    if(dequeue_size(dq) != 0) {
      return EXIT_FAILURE;
    }
    dequeue_reset(dq);
    // Test push back, pop back.
    puts("(b, b)");
        for(size_t i = 0; i < l; i++) {
      dequeue_push_back(dq, &TEST_NUMS[i]);
    }
    for(size_t i = 0; i < l; i++) {
      int result = 0;
      dequeue_pop_back(dq, &result);
      if(result != TEST_NUMS[l - i - 1]) {
        return EXIT_FAILURE;
      }
    }
    if(dequeue_size(dq) != 0) {
      return EXIT_FAILURE;
    }
    dequeue_reset(dq);
  }
  dequeue_destroy(dq);
  return EXIT_SUCCESS;
}