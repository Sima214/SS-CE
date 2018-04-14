#ifndef SSCE_CLOCK_H
#define SSCE_CLOCK_H
#include <stdint.h>
#if defined(__linux__)
  #define INTERNAL_STORAGE 2
#elif defined(__APPLE__)
  #define INTERNAL_STORAGE 0
#elif defined(_WIN32)
  #define INTERNAL_STORAGE 1
#else
  #error "Unknown platform."
#endif
/*
 * Cross platform clock intended for profiling code.
 * All the captured information is in
 * milliseconds unless otherwise stated.
 */
typedef struct {
  //Internal counters. Please ignore.
  int64_t intcnt[INTERNAL_STORAGE];
  //How many samples have been taken since last reset.
  int64_t count_query;
  //Total time that has passed since last reset.
  double delta_sum;
  //Last measurement.
  float delta;
  //Mean delta since last reset.
  float avg;
  //Smallest delta since last reset.
  float min;
  //Biggest delta since last reset.
  float max;
} PerfClock;
/*
 * This function should be called once
 * before ssce_delay is used.
 * Otherwise the result is undefined.
 */
extern void ssce_delay_hint();
/*
 * Creates a delay for the exact amount
 * of time requested. Only for small delays.
 * The argument is in microseconds and
 * it must be smaller than 1000000.
 */
extern void ssce_delay(int64_t usecs);
/*
 * Pause execution for the milliseconds
 * specified in the first argument.
 */
extern void ssce_msleep(int64_t msecs);
/*
 * Start measuring.
 */
extern void ssce_start(PerfClock*);
/*
 * Stops measurement and calculates
 * new delta and updates statistics.
 */
extern void ssce_stop(PerfClock*);
/*
 * Resets clock counters and statistics.
 * This function should also be called
 * to initialize a PerfClock structure.
 */
extern void ssce_reset(PerfClock*);
#endif /*SSCE_CLOCK_H*/