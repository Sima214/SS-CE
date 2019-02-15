/*
 * OSX initialization script.
 */
#include <Config.h>
#include <Modules.h>
#include <clock/Clock.h>
#include <logger/Logger.h>
#include <string/Strings.h>

/*
 * Early init procedure for Linux.
 */
static void __attribute__((constructor))
ssce_init(void) {
  #ifndef NDEBUG
    native_puts("Loading shared library ssce[" SSCE_VERSION "]");
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_init();
  #endif
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    setup_log_file();
  #endif
}

/*
 * Exit procedure for Linux.
 */
static void __attribute__((destructor))
ssce_exit(void) {
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    close_log_file();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_exit();
  #endif
  #ifndef NDEBUG
    native_puts("Unloaded shared library ssce[" SSCE_VERSION "]");
  #endif
}