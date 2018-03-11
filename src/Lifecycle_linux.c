/*
 * Linux initialization script.
 */
#include <Config.h>
#include <stdio.h>

void __attribute__((constructor)) libStart(void) {
  #ifndef NDEBUG
  puts("Loading shared library ssce[" SSCE_VERSION "]");
  #endif
}
void __attribute__((destructor)) libEnd(void) {
  #ifndef NDEBUG
  puts("Unloading shared library ssce[" SSCE_VERSION "]");
  #endif
}