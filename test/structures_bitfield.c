#include "test_utils.h"

#include <Bitfield.h>
#include <GAlloc.h>
#include <Logger.h>
#include <Macros.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define STRUCT_SIZE 64

static const size_t SET_BITS[] = {
  1 * sizeof(size_t),
  4 * sizeof(size_t),
  12 * sizeof(size_t),
  37 * sizeof(size_t),
  39 * sizeof(size_t),
  45 * sizeof(size_t),
  55 * sizeof(size_t),
  58 * sizeof(size_t)
};

int bench() {
  logger_set_level(LOGGER_INFO);
  const size_t size = bitfield_size(16 * 1024);
  void* data = malloc(size);
  // size_t* tmp = malloc(sizeof(size_t));
  fill_garbage(data, size);
  Bitfield bt;
  bitfield_init(&bt, data, size);
  for(size_t c = 0; c < 512 * 1024; c++) {
    bitfield_for_each(&bt, 1, 1, {
      // *tmp = bit_index;
      asm ("nop; mov %0,%0": : "r" (bit_index));
      //logger_logv("Bench test: %zu", bit_index);
    })
  }
  bitfield_deinit(&bt);
  return EXIT_SUCCESS;
}

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  if(argc > 1) {
    return bench();
  }
  void* data = malloc(STRUCT_SIZE);
  Bitfield bt;
  if(bitfield_init(&bt, data, STRUCT_SIZE)) {
    return EXIT_FAILURE;
  }
  bitfield_clear_all(&bt);
  for(size_t i = 0; i < sizeof(SET_BITS) / sizeof(size_t); i++) {
    bitfield_toggle(&bt, SET_BITS[i]);
  }
  for(size_t i = 0; i < sizeof(SET_BITS) / sizeof(size_t); i++) {
    if(!bitfield_get(&bt, SET_BITS[i])) {
      return EXIT_FAILURE;
    }
  }
  const size_t* last_set_bit = SET_BITS;
  size_t bits_set = 0;
  bitfield_for_each(&bt, -1, 0, {
    if(bit_value) {
      if(bit_index != *last_set_bit) {
        return EXIT_FAILURE;
      }
      last_set_bit++;
      bits_set++;
    }
  });
  if(bits_set != (sizeof(SET_BITS) / sizeof(size_t))) {
    return EXIT_FAILURE;
  }
  bitfield_deinit(&bt);
  return EXIT_SUCCESS;
}