#ifndef SSCE_CLOCK_HPP
#define SSCE_CLOCK_HPP
#include <Macros.h>
C_DECLS_START
#include <Clock.h>
C_DECLS_END
namespace ssce{
class PerformanceClock {
  public:
    /*
     * Constructor.
     */
    PerformanceClock(): pc() {
      ssce_reset(&pc);
    }
    /*
     * Start measuring.
     */
    void start() {
      ssce_start(&pc);
    }
    /*
     * Stops measurement and calculates
     * new delta and updates statistics.
     */
    void stop() {
      ssce_stop(&pc);
    }
    /*
     * Resets clock counters and statistics.
     * This function should also be called
     * to initialize a PerfClock structure.
     */
    void reset() {
      ssce_reset(&pc);
    }
  private:
    PerfClock pc;
};
/*
 * This function should be called once
 * before delay is used.
 * Otherwise the result is undefined.
 */
extern void delay_hint();
/*
 * Creates a delay for the exact amount
 * of time requested. Only for small delays.
 * The argument is in microseconds and
 * it must be smaller than 1000000.
 */
extern void delay(int64_t usecs);
/*
 * Pause execution for the milliseconds
 * specified in the first argument.
 */
void msleep(int64_t t) {
  ssce_msleep(t);
}
}
#endif /*SSCE_CLOCK_HPP*/