#include "PrimeGenerator.h"

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// List of found integers.
static uintmax_t* primegen_list;
static size_t* primegen_list_len;
// End of generated numbers.
static uintmax_t primegen_end;

// Thread control.
pthread_t primegen_thr;
pthread_mutex_t primegen_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t primegen_cond = PTHREAD_COND_INITIALIZER;
static volatile int primegen_thr_exit;

prime_t primegen_get_next(prime_t state) {
  return (prime_t){0, 0};
}

// Thread generator.
void* internal_primegen_main(void* unused) {
  pthread_setname_np(pthread_self(), "SSCE_PRIMEGEN");
  pthread_mutex_lock(&primegen_mutex);
  while(!primegen_thr_exit) {
    // Do work.
    EARLY_TRACE("Primegen working...");
    // TODO:
    // Wait until more work is needed.
    EARLY_TRACE("Primegen waiting...");
    // Releases mutex, so primegen_get_next can execute.
    pthread_cond_wait(&primegen_cond, &primegen_mutex);
  }
  pthread_mutex_unlock(&primegen_mutex);
  EARLY_TRACE("Primegen exiting...");
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
  // Wait until thread is done with its work.
  pthread_mutex_lock(&primegen_mutex);
  primegen_thr_exit = 1;
  pthread_cond_broadcast(&primegen_cond);
  pthread_mutex_unlock(&primegen_mutex);
  pthread_join(primegen_thr, NULL);
}