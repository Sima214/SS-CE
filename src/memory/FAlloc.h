#ifndef SSCE_FALLOC_H
#define SSCE_FALLOC_H
/**
 * @file
 * @brief This is header provides a per-thread stack based allocator.
 * It's function and performance is similar to alloca and VLA,
 * but warns you before you overflow the stack.
 */

#include <Macros.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Stores information about the thread local extra stack.
 */
typedef struct {
  /**
   * The start address of the allocated stack.
   */
  void* start;
  /**
   * How many bytes are currently getting used by fast_malloc?
   */
  size_t usage;
  /**
   * How much ram have we allocated?
   */
  size_t allocated;
  #if defined(_WIN32)
    /**
     * How much virtual address space have we allocated?
     * This is only needed on Windows.
     */
    size_t reserved;
  #endif
} ThreadLocalStack;

/**
 * Internal api usage only!
 * Returns info about allocated thread local stack.
 */
EXPORT_API ThreadLocalStack* falloc_get_tls();

/**
 * Internal api usage only!
 * Ensures that there are at least \p l free in \p tls.
 * Returns non-zero value on error.
 */
EXPORT_API int falloc_ensure_space(ThreadLocalStack* tls, size_t l);

/**
 * Allocates \p l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned sizeof(int) bytes.
 * If not enough memory could be allocated, then NULL is returned.
 */
EXPORT_API void* falloc_malloc(size_t l) MARK_MALLOC(1);

/**
 * Allocates \p l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned at \p align bytes.
 * \p align must be a power of 2.
 * If not enough memory could be allocated, then NULL is returned.
 */
EXPORT_API void* falloc_malloc_aligned(size_t l, size_t align) MARK_MALLOC_ALIGNED(2, 1);

/**
 * The equivalent of free for \ref falloc_malloc_aligned.
 */
EXPORT_API void falloc_free(void* ptr);

/**
 * Initializes thread local keys.
 */
void internal_falloc_init();

/**
 * Cleans up thread local keys.
 */
void internal_falloc_exit();

#endif /*SSCE_FALLOC_H*/