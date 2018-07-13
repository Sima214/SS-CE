#include "test_utils.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <Strings.h>
#include <Clock.h>

static int test_swap(void* orig0, void* orig1, void* swap0, void* swap1, size_t len) {
  return memcmp(orig0, swap1, len)==0 && memcmp(orig1, swap0, len)==0;
}

static int stress(uint8_t* garbage0, uint8_t* garbage1, uint8_t* test0, uint8_t* test1, size_t len) {
  memcpy(test0, garbage0, len);
  memcpy(test1, garbage1, len);
  PerfClock pc;
  for(size_t cl=1; cl<=KBYTES(8); cl+=1) {
    ssce_reset(&pc);
    for(int i=0; i<128; i++) {
      ssce_start(&pc);
      for(int j=0; j<4096; j++) {
        memswap(test0, test1, cl);
      }
      ssce_stop(&pc);
    }
    // Report results.
    printf("%5zu byte blocks: %6.4f | %6.4f | %6.4f\n", cl, pc.avg, pc.min, pc.max);
    //printf("%6.4f, %6.4f, %6.4f\n", pc.avg, pc.min, pc.max);
    //printf("%f\n", pc.avg);
  }
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  static uint8_t garbage0[KBYTES(8)];
  static uint8_t garbage1[KBYTES(8)];
  static uint8_t test0[KBYTES(8)];
  static uint8_t test1[KBYTES(8)];
  fill_garbage(garbage0, sizeof(garbage0));
  fill_garbage(garbage1, sizeof(garbage1));
  // Do performance testing.
  if(argc > 1) {
    return stress(garbage0, garbage1, test0, test1, KBYTES(8));
  }
  for(size_t cl=1; cl<=KBYTES(8); cl++) {
    printf("Testing %zu byte blocks\n", cl);
    memcpy(test0, garbage0, cl);
    memcpy(test1, garbage1, cl);
    // Call method being tested.
    memswap(test0, test1, cl);
    // Confirm result.
    if(!test_swap(garbage0, garbage1, test0, test1, cl)) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}