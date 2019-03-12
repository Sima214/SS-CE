#include "Clock.h"

#include <core/ntdll.h>
#include <math/MinMax.h>
#include <string/SStrings.h>

#include <float.h>
#include <stdint.h>
#include <windows.h>

/**
 * This should give us approximately 300Hz clock resolution.
 */
static const ULONG timer_preferred = 33333;
static int64_t clock_frequency;
static ULONG timer_reset;

void clock_delay(int64_t usecs) {
  if(COLD_BRANCH(usecs <= 0 || usecs >= 1000000)) {
    native_puts("clock_delay input out of bounds!");
    return;
  }
  // Valid input - continue.
  LARGE_INTEGER delay;
  // Convert input to relative 100ns units.
  delay.QuadPart = -((usecs * MICRO2NANO) / 100);
  NtDelayExecution(FALSE, &delay);
}

void clock_msleep(int64_t msecs) {
  Sleep(msecs);
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
  nano_delta = nano_delta - pc->internal[0];
  double delta = ((double)nano_delta) / clock_frequency;
  pc->count_query++;
  pc->delta_sum += delta;
  pc->avg = pc->delta_sum / pc->count_query;
  pc->delta = delta;
  if(delta > pc->max) {
    pc->max = delta;
  }
  if(delta < pc->min) {
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
  // Increase timer resolution.
  ULONG timer_min;
  ULONG timer_max;
  NtQueryTimerResolution(&timer_min, &timer_max, &timer_reset);
  ULONG timer_request = math_max(timer_min, timer_preferred);
  NtSetTimerResolution(timer_request, TRUE, NULL);
  // Get clock frequency.
  LARGE_INTEGER x;
  QueryPerformanceFrequency(&x);
  clock_frequency = x.QuadPart / 1000;
}

void internal_clock_exit() {
  // Reset timer resolution.
  NtSetTimerResolution(timer_reset, TRUE, NULL);
}