#include "test_utils.h"

#include <Clock.h>
#include <Macros.h>
#include <Memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
  #include <inttypes.h>
  #ifdef _WIN64
    #define SIZET_FMT PRIu64
  #else
    #define SIZET_FMT PRIu32
  #endif
#else
  #define SIZET_FMT "zu"
#endif

#define UNALIGNED_MAX_OFFSET 512
#define UNALIGNED_SAMPLE_SIZE 1024

static int test_swap(void* orig0, void* orig1, void* swap0, void* swap1, size_t len) {
  return memcmp(orig0, swap1, len) == 0 && memcmp(orig1, swap0, len) == 0;
}

static int stress(uint8_t* garbage0, uint8_t* garbage1, uint8_t* test0, uint8_t* test1, size_t len) {
  memcpy(test0, garbage0, len);
  memcpy(test1, garbage1, len);
  PerfClock pc;
  for(size_t cl = 1; cl <= KBYTES(8); cl += 1) {
    clock_reset(&pc);
    for(int i = 0; i < 128; i++) {
      clock_start(&pc);
      for(int j = 0; j < 4096; j++) {
        memswap(test0, test1, cl);
      }
      clock_stop(&pc);
    }
    // Report results.
    printf("%5"SIZET_FMT" byte blocks: %6.4f | %6.4f | %6.4f\n", cl, pc.avg, pc.min, pc.max);
    //printf("%6.4f, %6.4f, %6.4f\n", pc.avg, pc.min, pc.max);
    //printf("%f\n", pc.avg);
  }
  return EXIT_SUCCESS;
}

int main(int argc, MARK_UNUSED char* argv[]) {
  srand(time(NULL));
  static uint8_t garbage0[KBYTES(8)];
  static uint8_t garbage1[KBYTES(8)];
  static uint8_t test0[KBYTES(8)];
  static uint8_t test1[KBYTES(8)];
  fill_garbage(garbage0, sizeof(garbage0));
  fill_garbage(garbage1, sizeof(garbage1));
  // Do performance testing(interactive only).
  if(argc > 1) {
    return stress(garbage0, garbage1, test0, test1, KBYTES(8));
  }
  // Standard testing.
  printf("Testing standard aligned blocks...\n");
  for(size_t cl = 1; cl <= KBYTES(8); cl++) {
    memcpy(test0, garbage0, cl);
    memcpy(test1, garbage1, cl);
    // Call method being tested.
    memswap(test0, test1, cl);
    // Confirm result.
    if(!test_swap(garbage0, garbage1, test0, test1, cl)) {
      printf("Errored %"SIZET_FMT" bytes!\n", cl);
      return EXIT_FAILURE;
    }
  }
  // Unaligned tests.
  printf("Testing unaligned %d byte blocks...\n", UNALIGNED_SAMPLE_SIZE);
  for(int i = 0; i <= UNALIGNED_MAX_OFFSET; i++) {
    for(int j = 0; j <= UNALIGNED_MAX_OFFSET; j++) {
      uint8_t* test0_u = test0 + i;
      uint8_t* test1_u = test1 + j;
      memcpy(test0_u, garbage0, UNALIGNED_SAMPLE_SIZE);
      memcpy(test1_u, garbage1, UNALIGNED_SAMPLE_SIZE);
      // Call method being tested.
      memswap(test0_u, test1_u, UNALIGNED_SAMPLE_SIZE);
      // Confirm result.
      if(!test_swap(garbage0, garbage1, test0_u, test1_u, UNALIGNED_SAMPLE_SIZE)) {
        printf("Errored at %d,%d offsets!\n", i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}