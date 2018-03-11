/*
 * Linux initialization script.
 */
#include <Config.h>
#include <stdio.h>

void __attribute__((constructor)) libStart(void) {
  #ifndef NDEBUG
  printf("Loading shared library ssce[" SSCE_VERSION "]");
  #endif
}
void __attribute__((destructor)) libEnd(void) {
  #ifndef NDEBUG
  printf("Unloading shared library ssce[" SSCE_VERSION "]");
  #endif
}