/*
 * Linux initialization script.
 */
#define SSCE_LOADER
#include <Config.h>
#include <logger/Logger.h>
#include <stdio.h>

void __attribute__((constructor)) libStart(void) {
  #ifndef NDEBUG
  puts("Loading shared library ssce[" SSCE_VERSION "]");
  #endif
  setup_log_file();
}
void __attribute__((destructor)) libEnd(void) {
  #ifndef NDEBUG
  puts("Unloading shared library ssce[" SSCE_VERSION "]");
  #endif
  close_log_file();
}