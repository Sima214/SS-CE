#include "test_utils.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <string/Strings.h>
#include <clock/Clock.h>

int test_swap(void* orig0, void* orig1, void* swap0, void* swap1, size_t len) {
  return memcmp(orig0, swap1, len)==0 && memcmp(orig1, swap0, len)==0;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  static uint8_t garbage0[KBYTES(16)];
  static uint8_t garbage1[KBYTES(16)];
  static uint8_t test0[KBYTES(16)];
  static uint8_t test1[KBYTES(16)];
  fill_garbage(garbage0, sizeof(garbage0));
  fill_garbage(garbage1, sizeof(garbage1));
  // Do performance testing.
  if(argc > 1) {
    memcpy(test0, garbage0, sizeof(garbage0));
    memcpy(test1, garbage1, sizeof(garbage0));
    PerfClock pc;
    for(size_t cl=1; cl<=KBYTES(16); cl++) {
      ssce_reset(&pc);
      for(int i=0; i<16; i++) {
        ssce_start(&pc);
        for(int j=0; j<1024*16; j++) {
          memswap_avx(test0, test1, cl);
        }
        ssce_stop(&pc);
      }
      // Report results.
      //printf("%5zu byte blocks: %6.4f | %6.4f | %6.4f\n", cl, pc.avg, pc.min, pc.max);
      printf("%6.4f, %6.4f, %6.4f\n", pc.avg, pc.min, pc.max);
    }
    return EXIT_SUCCESS;
  }
  for(size_t cl=1; cl<=KBYTES(16); cl++) {
    printf("Testing %zu byte blocks\n", cl);
    memcpy(test0, garbage0, cl);
    memcpy(test1, garbage1, cl);
    // Call method being tested.
    memswap_sse2(test0, test1, cl);
    // Confirm result.
    if(!test_swap(garbage0, garbage1, test0, test1, cl)) {
      return EXIT_FAILURE;
    }
  }
  //PerfClock pc;
  //ssce_reset(&pc);
  //ssce_start(&pc);
  //ssce_stop(&pc);
  return EXIT_SUCCESS;
}