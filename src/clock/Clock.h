#ifndef SSCE_CLOCK_H
#define SSCE_CLOCK_H
/**
 * @file
 * @brief High performance time measurement and delays.
 */

#include <Macros.h>

#include <stdint.h>

#define MILLI2MICRO 1000
#define MICRO2NANO 1000
#define MSEC2NANO 1000000
#define SEC2NANO 1000000000

#if defined(__linux__)
  #define INTERNAL_STORAGE 2
#elif defined(__APPLE__)
  #define INTERNAL_STORAGE 2
#elif defined(_WIN32)
  #define INTERNAL_STORAGE 1
#else
  #error "Unsupported platform!"
#endif

/**
 * Cross platform high accuracy clock 
 * intended for profiling code.
 * All the captured information is in
 * milliseconds unless otherwise stated.
 */
typedef struct {
  /** Internal counters. Please ignore. */
  int64_t internal[INTERNAL_STORAGE];
  /** How many samples have been taken since last reset. */
  int64_t count_query;
  /** Total time that has passed since last reset. */
  double delta_sum;
  /** Last measurement. */
  float delta;
  /** Mean delta since last reset. */
  float avg;
  /** Smallest delta since last reset. */
  float min;
  /** Biggest delta since last reset. */
  float max;
} PerfClock;

/**
 * Creates a delay for the exact amount
 * of time requested. Only for small delays.
 * The argument is in microseconds and
 * it must be smaller than 1000000(1 second).
 */
EXPORT_API void clock_delay(int64_t usecs);

/**
 * Pause execution for the milliseconds
 * specified in the first argument.
 * This is less accurate then clock_delay,
 * but it usually has a more
 * efficient implementation.
 */
EXPORT_API void clock_msleep(int64_t msecs);

/**
 * Start measuring.
 */
EXPORT_API void clock_start(PerfClock*);

/**
 * Stops measurement and calculates
 * new delta and updates statistics.
 */
EXPORT_API void clock_stop(PerfClock*);

/**
 * Resets clock counters and statistics.
 * This function should also be called
 * to initialize a PerfClock structure.
 */
EXPORT_API void clock_reset(PerfClock*);

/**
 * Used internally for initializing this module.
 */
void internal_clock_init();

/**
 * Used internally for cleaning up this module.
 */
void internal_clock_exit();

#endif /*SSCE_CLOCK_H*/