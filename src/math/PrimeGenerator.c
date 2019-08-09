#include "PrimeGenerator.h"

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// List of found integers.
static uintmax_t* primegen_list;
// End of generated numbers.
static uintmax_t primegen_end;

// Thread control.
pthread_t primegen_thr;
static volatile int primegen_thr_exit;

prime_t primegen_get_next(prime_t state) {
  return (prime_t){0, 0};
}

// Thread generator.
void* internal_primegen_main(void* unused) {
  pthread_setname_np(pthread_self(), "PRIMEGEN");
  while(!primegen_thr_exit) {
  }
  return NULL;
}

void internal_primegen_init() {
  int ret = pthread_create(&primegen_thr, NULL, internal_primegen_main, NULL);
  if(COLD_BRANCH(ret != 0)) {
    EARLY_TRACEF("Could not create primegen thread code: 0x%x!", ret);
    abort();
  }
}

void internal_primegen_exit() {
  primegen_thr_exit = 1;
  pthread_join(primegen_thr, NULL);
}