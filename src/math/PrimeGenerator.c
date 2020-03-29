#include "PrimeGenerator.h"

#include <Macros.h>
#include <MathExtra.h>
#include <MinMax.h>
#include <core/PosixThreads.h>
#include <core/Runtime.h>
#include <memory/FAlloc.h>
#include <memory/GAlloc.h>
#include <structures/Bitfield.h>

#include <errno.h>
#include <math.h>
#include <sched.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define PRIMEGEN_STATIC_END 6

// List of found integers.
static uintmax_t* volatile primegen_list;
static volatile size_t primegen_list_len;

// Thread control.
pthread_t primegen_thr;
pthread_mutex_t primegen_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t primegen_cond = PTHREAD_COND_INITIALIZER;
pthread_spinlock_t primegen_list_lock;
static volatile int primegen_thr_exit;
static volatile int primegen_thr_ready;

prime_t primegen_get_next(prime_t state) {
  state.last_prime_index++;
  int must_wait_for_result = 0;
  // Thread might be running, so sync only on the result (primegen_list, primegen_list_len).
  pthread_spin_lock(&primegen_list_lock);
  if(state.last_prime_index < primegen_list_len) {
    // We can retrieve the next value.
    state.last_prime = primegen_list[state.last_prime_index];
  } else {
    // We request faster than we can generate.
    must_wait_for_result = 1;
  }
  pthread_spin_unlock(&primegen_list_lock);
  if(COLD_BRANCH(must_wait_for_result)) {
    EARLY_TRACE("primegen_get_next waiting!");
    pthread_mutex_lock(&primegen_mutex);
    // Thread was either not running, or we waited until it finished running.
    if(state.last_prime_index < primegen_list_len) {
      // Got it.
      state.last_prime = primegen_list[state.last_prime_index];
      return state;
    } else {
      // Thread was not running...
      EARLY_TRACE("primegen_get_next still waiting...");
      pthread_cond_broadcast(&primegen_cond);
      pthread_mutex_unlock(&primegen_mutex);
      // Wait until the thread has finished this loop.
      while(1) {
        int mutex_state = pthread_mutex_trylock(&primegen_mutex);
        if(mutex_state == EBUSY) {
          // Wait until thread finishes.
          pthread_mutex_lock(&primegen_mutex);
          if(state.last_prime_index < primegen_list_len) {
            state.last_prime = primegen_list[state.last_prime_index];
            pthread_mutex_unlock(&primegen_mutex);
            return state;
          } else {
            // Give up at this point.
            EARLY_TRACE("primegen's thread did not generate primes!");
            abort();
          }
        } else {
          pthread_mutex_unlock(&primegen_mutex);
          // Thread has already finished.
          if(state.last_prime_index < primegen_list_len) {
            state.last_prime = primegen_list[state.last_prime_index];
            return state;
          } else {
            // Thread hasn't even started yet!
            EARLY_TRACE("primegen_get_next still waiting...");
            sched_yield();
          }
        }
      }
    }
  } else {
    // If we are getting low of primes, request next batch from thread.
    if((primegen_list_len - state.last_prime_index) <= 32) {
      int mutex_locked = pthread_mutex_trylock(&primegen_mutex);
      if(mutex_locked == 0) {
        EARLY_TRACE("primegen_get_next requesting new primes...");
        pthread_cond_broadcast(&primegen_cond);
        pthread_mutex_unlock(&primegen_mutex);
        sched_yield();
      } else if(mutex_locked != EBUSY) {
        EARLY_TRACE("primegen invalid mutex state!");
      }
    }
  }
  return state;
}

/**
 * First pass using previously generated values.
 */
void internal_primegen_algo_pre(const Bitfield* bt, const uintmax_t start, const uintmax_t end, const uintmax_t fast_bound) {
  for(size_t i = 2; i < primegen_list_len; i++) {
    // Use already generated primes, starting from 3.
    uintmax_t current_prime = primegen_list[i];
    if(current_prime <= fast_bound) {
      break;
    }
    for(uintmax_t not_prime = current_prime * current_prime; not_prime <= end; not_prime += current_prime) {
      // Convert to bit index.
      size_t not_prime_index = (not_prime - start) / 2;
      bitfield_clear(bt, not_prime_index);
    }
  }
}

/**
 * Partial sieve pass.
 */
void internal_primegen_algo_main(Bitfield* bt, const uintmax_t start, const uintmax_t end, const uintmax_t fast_bound, const size_t job_size) {
  for(size_t i = 0; i < job_size; i++) {
    // If current number is prime:
    if(bitfield_get(bt, i)) {
      uintmax_t current_prime = start + i * 2;
      if(current_prime <= fast_bound) {
        break;
      }
      for(uintmax_t not_prime = current_prime * current_prime; not_prime <= end; not_prime += current_prime) {
        // Convert to bit index.
        size_t not_prime_index = (not_prime - start) / 2;
        bitfield_clear(bt, not_prime_index);
      }
    }
  }
}

// Thread main.
void* internal_primegen_main(MARK_UNUSED void* input) {
  pthread_setname_self("SSCE_PRIMEGEN");
  // End of generated numbers.
  uintmax_t end = PRIMEGEN_STATIC_END;
  // Allocate buffer which fits exactly in cache.
  Runtime* rt = ssce_get_runtime();
  size_t job_size = math_max(rt->cpu_cache_size_l1d, 4096U);
  // TODO: cache line size may not always be valid.
  void* bitfield_store = falloc_malloc_aligned(job_size, rt->cpu_cache_alignment);
  // Allocate bitfield.
  Bitfield bitfield;
  bitfield_init(&bitfield, bitfield_store, job_size);
  bitfield_set_all(&bitfield);
  job_size *= __CHAR_BIT__;
  // Thread main loop.
  pthread_mutex_lock(&primegen_mutex);
  primegen_thr_ready = 1;
  while(HOT_BRANCH(!primegen_thr_exit)) {
    // Wait until more work is needed.
    EARLY_TRACE("Primegen waiting...");
    // Releases mutex, so primegen_get_next can execute.
    pthread_cond_wait(&primegen_cond, &primegen_mutex);
    if(COLD_BRANCH(primegen_thr_exit)) break;
    // Do work.
    uintmax_t start = end + 1;
    end = end + job_size * 2;
    uintmax_t fast_bound = ssce_sqrt(end);
    EARLY_TRACEF("Primegen working at [%ju, %ju, %ju]...", start, fast_bound, end);
    // Algorithm used: Sieve of Eratosthenes with square bound, bitarray, odd-only optimizations.
    internal_primegen_algo_pre(&bitfield, start, end, fast_bound);
    if(COLD_BRANCH(primegen_thr_exit)) break;
    internal_primegen_algo_main(&bitfield, start, end, fast_bound, job_size);
    if(COLD_BRANCH(primegen_thr_exit)) break;
    // Prepare for prime extraction.
    const size_t old_primes = primegen_list_len;
    size_t potential_new_primes = math_max(((double)end) / log((double)end) * 1.2, old_primes + 8);
    size_t new_primes = old_primes;
    pthread_spin_lock(&primegen_list_lock);
    // Store results.
    primegen_list = realloc(primegen_list, potential_new_primes * sizeof(uintmax_t));
    bitfield_for_each(&bitfield, 1, 0, {
      if(COLD_BRANCH(new_primes == potential_new_primes)) {
        EARLY_TRACE("primegen did not preallocate enough space!");
        potential_new_primes += 8;
        primegen_list = realloc(primegen_list, potential_new_primes * sizeof(uintmax_t));
      }
      uintmax_t extracted_prime = start + bit_index * 2;
      primegen_list[new_primes] = extracted_prime;
      new_primes++;
    });
    primegen_list_len = new_primes;
    pthread_spin_unlock(&primegen_list_lock);
    // Clean up and prepare for next work.
    bitfield_set_all(&bitfield);
  }
  pthread_mutex_unlock(&primegen_mutex);
  bitfield_deinit(&bitfield);
  falloc_free(bitfield_store);
  EARLY_TRACE("Primegen exiting...");
  return NULL;
}

void internal_primegen_init() {
  pthread_spin_init(&primegen_list_lock, PTHREAD_PROCESS_PRIVATE);
  primegen_thr_ready = 0;
  int ret = pthread_create(&primegen_thr, NULL, internal_primegen_main, NULL);
  while (!primegen_thr_ready) {
    sched_yield();
  }
  primegen_list_len = 4;
  primegen_list = malloc(primegen_list_len * sizeof(uintmax_t));
  primegen_list[0] = 1;
  primegen_list[1] = 2;
  primegen_list[2] = 3;
  primegen_list[3] = 5;
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
  pthread_spin_destroy(&primegen_list_lock);
}