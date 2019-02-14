#include "Clock.h"

#include <string/Strings.h>

#include <stdint.h>
#include <float.h>
#include <Windows.h>

static int64_t clock_frequency = 0;

void clock_delay(int64_t usecs) {
  #ifndef NDEBUG
    if(usecs <= 0 || usecs >= 1000000) {
      native_puts("clock_delay input out of bounds!");
      return;
    }
  #endif
  // Valid input - continue.
}

void clock_msleep(int64_t msecs) {
}

void clock_start(PerfClock* pc) {
  LARGE_INTEGER x;
  QueryPerformanceCounter(&x);
  pc->internal[0] = x.QuadPart;
}

void clock_stop(PerfClock* pc) {
  LARGE_INTEGER x;
  QueryPerformanceCounter(&x);
  int64_t nano_delta = x.QuadPart;
  nano_delta = nano_delta - pc->last_query;
  double delta = ((double)nano_delta)/FREQUENCY;
  pc->count_query++;
  pc->delta_sum += delta;
  pc->avg = pc->delta_sum/pc->count_query;
  pc->delta = delta;
  if(delta > pc->max){
    pc->max = delta;
  }
  if(delta < pc->min){
    pc->min = delta;
  }
}

void clock_reset(PerfClock* pc) {
  pc->count_query = 0;
  pc->delta_sum = 0;
  pc->max = FLT_MIN;
  pc->min = FLT_MAX;
}

void internal_clock_init() {
  // Get clock frequency.
  LARGE_INTEGER x;
  QueryPerformanceFrequency(&x);
  clock_frequency = x.QuadPart/1000;
}

void internal_clock_exit() {
  /* No action required. */
}