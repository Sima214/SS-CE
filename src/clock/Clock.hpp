#ifndef SSCE_CLOCK_HPP
#define SSCE_CLOCK_HPP
/**
 * @file
 * @brief High performance time measurement and delays.
 */

#include <Macros.h>
C_DECLS_START
#include <Clock.h>
C_DECLS_END

namespace ssce {

/**
 * High accuracy stopwatch.
 */
class PerformanceClock {
public:
  /**
   * Constructor.
   */
  PerformanceClock(): pc() {
    clock_reset(&pc);
  }
  /**
   * Start measuring.
   */
  void start() {
    clock_start(&pc);
  }
  /**
   * Stops measurement and calculates
   * new delta and updates statistics.
   */
  void stop() {
    clock_stop(&pc);
  }
  /**
   * Resets clock counters and statistics.
   * This function should also be called
   * to initialize a PerfClock structure.
   */
  void reset() {
    clock_reset(&pc);
  }

private:
  /**
   * Interface with the C portion.
   */
  PerfClock pc;
};

/**
 * Creates a delay for the exact amount
 * of time requested. Only for small delays.
 * The argument is in microseconds and
 * it must be smaller than 1000000(1 second).
 */
inline void delay(int64_t usecs) {
  clock_delay(usecs);
}

/**
 * Pause execution for the milliseconds
 * specified in the first argument.
 * This is less accurate then delay,
 * but it usually has a more
 * efficient implementation.
 */
inline void msleep(int64_t t) {
  clock_msleep(t);
}

} // namespace ssce
#endif /*SSCE_CLOCK_HPP*/