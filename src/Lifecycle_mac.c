/*
 * OSX initialization script.
 */
#include <Config.h>
#include <Macros.h>
#include <Modules.h>
#include <clock/Clock.h>
#include <logger/Logger.h>

/*
 * Early init procedure for MacOS.
 */
static void __attribute__((constructor))
ssce_init(void) {
  EARLY_TRACE("Loading shared library ssce[" SSCE_VERSION "]");
  #if defined(MODULE_CLOCK)
    internal_clock_init();
  #endif
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    setup_log_file();
  #endif
}

/*
 * Exit procedure for MacOS.
 */
static void __attribute__((destructor))
ssce_exit(void) {
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    close_log_file();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_exit();
  #endif
  EARLY_TRACE("Unloaded shared library ssce[" SSCE_VERSION "]");
}