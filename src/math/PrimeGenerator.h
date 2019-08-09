#ifndef SSCE_PRIMEGENERATOR
#define SSCE_PRIMEGENERATOR
/**
* @file
* @brief 
*/

#include <Macros.h>

#include <stddef.h>
#include <stdint.h>

/**
 * Prime generator state.
 */
typedef struct {
  uintmax_t last_prime;
  size_t last_prime_index;
} prime_t;

/**
 * Generates a new prime state used to
 * generate a series of prime numbers.
 */
static inline FORCE_INLINE MARK_UNUSED prime_t primegen_new() {
  return (prime_t){0, SIZE_MAX};
}

/**
 * Gets the last prime number generated
 * by \ref primegen_get_next.
 */
static inline FORCE_INLINE MARK_UNUSED uintmax_t primegen_extract(prime_t state) {
  return state.last_prime;
}

/**
 * Generates and returns the next prime state.
 */
EXPORT_API prime_t primegen_get_next(prime_t state);

/**
 * Used internally for initializing this module.
 */
void internal_primegen_init();

/**
 * Used internally for cleaning up this module.
 */
void internal_primegen_exit();

#endif /*SSCE_PRIMEGENERATOR*/