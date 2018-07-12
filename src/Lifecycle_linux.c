/*
 * Linux initialization script.
 */
#define SSCE_LOADER
#include <Config.h>
#include <Modules.h>
#include <logger/Logger.h>
#include <stdio.h>

/*
 * Early init procedure for Linux.
 */
static void __attribute__((constructor)) ssce_init(void) {
  #ifndef NDEBUG
  puts("Loading shared library ssce[" SSCE_VERSION "]");
  #endif
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
  setup_log_file();
  #endif
}

/*
 * Exit procedure for Linux.
 */
static void __attribute__((destructor)) ssce_exit(void) {
  #ifndef NDEBUG
  puts("Unloading shared library ssce[" SSCE_VERSION "]");
  #endif
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
  close_log_file();
  #endif
}