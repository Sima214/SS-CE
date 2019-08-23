/*
 * OSX initialization script.
 */
#include <Config.h>
#include <Macros.h>
#include <Modules.h>
#include <Runtime.h>
#include <clock/Clock.h>
#include <logger/Logger.h>
#include <math/PrimeGenerator.h>
#include <memory/FAlloc.h>

/*
 * Early init procedure for MacOS.
 */
static void __attribute__((constructor))
ssce_init(void) {
  EARLY_TRACE("Loading shared library ssce[" SSCE_VERSION "]");
  #if defined(MODULE_MEMORY)
    internal_falloc_init();
  #endif
  internal_runtime_init();
  #if defined(MODULE_CLOCK)
    internal_clock_init();
  #endif
  #if defined(MODULE_LOGGER)
    internal_logger_init();
  #endif
  #if defined(MODULE_MATH)
    internal_primegen_init();
  #endif
}

/*
 * Exit procedure for MacOS.
 */
static void __attribute__((destructor))
ssce_exit(void) {
  #if defined(MODULE_LOGGER)
    internal_logger_exit();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_exit();
  #endif
  #if defined(MODULE_MEMORY)
    internal_falloc_exit();
  #endif
  EARLY_TRACE("Unloaded shared library ssce[" SSCE_VERSION "]");
}