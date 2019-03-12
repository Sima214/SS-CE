#include "Clock.h"

#include <string/SStrings.h>
#include <Macros.h>

#include <errno.h>
#include <float.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

static clock_serv_t kernel_clock;

void clock_delay(int64_t usecs) {
  if(COLD_BRANCH(usecs <= 0 || usecs >= 1000000)) {
    EARLY_TRACE("clock_delay input out of bounds!");
    return;
  }
  // Valid input - continue.
  struct timespec towait = {0, usecs * MICRO2NANO};
  struct timespec remain = {0, 0};
  while(nanosleep(&towait, &remain) == -1 && errno == EINTR) {
    towait = remain;
  }
}

void clock_msleep(int64_t msecs) {
  usleep(msecs * MILLI2MICRO);
}

void clock_start(PerfClock* pc) {
  mach_timespec_t tp;
  clock_get_time(kernel_clock, &tp);
  pc->internal[0] = tp.tv_nsec;
  pc->internal[1] = tp.tv_sec;
}

void clock_stop(PerfClock* pc) {
  mach_timespec_t tp;
  clock_get_time(kernel_clock, &tp);
  int64_t nano_delta = (tp.tv_nsec - pc->internal[0]) + SEC2NANO * (tp.tv_sec - pc->internal[1]);
  double delta = ((double)nano_delta) / MSEC2NANO;
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
  // Request a kernel clock object.
  host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &kernel_clock);
}

void internal_clock_exit() {
  // Free kernel object.
  mach_port_deallocate(mach_task_self(), kernel_clock);
}