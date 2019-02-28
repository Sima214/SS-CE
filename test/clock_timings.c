#include "test_utils.h"

#include <Clock.h>
#include <Macros.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ITER_COUNT_LARGE 32

#define ITER_COUNT_SMALL 256

#define print_stats(pc, delay) printf("%d\t: %.3f\t%.3f\t%.3f\n", delay, pc.min, pc.avg, pc.max)

#define measure_large(pc, delay)              \
  for(int i = 0; i < ITER_COUNT_LARGE; i++) { \
    clock_start(&pc);                         \
    clock_msleep(delay);                      \
    clock_stop(&pc);                          \
  }                                           \
  print_stats(pc, delay);

#define measure_small(pc, delay)              \
  for(int i = 0; i < ITER_COUNT_SMALL; i++) { \
    clock_start(&pc);                         \
    clock_delay(delay);                       \
    clock_stop(&pc);                          \
  }                                           \
  print_stats(pc, delay);

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  PerfClock pc;
  clock_reset(&pc);
  printf("T\t: MIN\tAVG\tMAX\n");
  measure_large(pc, 33);
  clock_reset(&pc);
  measure_large(pc, 16);
  clock_reset(&pc);
  measure_large(pc, 7);
  clock_reset(&pc);
  measure_small(pc, 6944);
  clock_reset(&pc);
  measure_small(pc, 4166);
  clock_reset(&pc);
  measure_small(pc, 3333);
  clock_reset(&pc);
  measure_small(pc, 1000);
  return EXIT_SUCCESS;
}
