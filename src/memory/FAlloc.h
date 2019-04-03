#ifndef SSCE_FALLOC_H
#define SSCE_FALLOC_H
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
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
EXPORT_API void* falloc_malloc(size_t l) MARK_MALLOC_SIMPLE(1);

#ifndef DOXYGEN
  #define FALLOC_MALLOC_ATTR MARK_UNUSED FORCE_INLINE MARK_MALLOC(2, 1)
#else
  #define FALLOC_MALLOC_ATTR
#endif

/**
 * Allocates \p l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned \p align bytes.
 * If not enough memory could be allocated, then NULL is returned.
 */
FALLOC_MALLOC_ATTR static inline void* falloc_malloc_aligned(size_t l, size_t align) {
  // Get thread local stack.
  ThreadLocalStack* tls = falloc_get_tls();
  // Calculate alignment.
  uintptr_t end = ((uintptr_t)tls->start) + tls->usage;
  size_t align_offset = end % align;
  if(align_offset != 0) {
    align_offset = align - align_offset;
  }
  size_t actual_len = align_offset + l;
  // Request bytes.
  if(COLD_BRANCH(falloc_ensure_space(tls, actual_len))) {
    EARLY_TRACE("Could not allocate fast ram!");
    return NULL;
  }
  EARLY_TRACE("Allocating fast ram!");
  // Calculate final address.
  void* final = (void*) (end + align_offset);
  // Update usage.
  tls->usage += actual_len;
  return final;
}

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

#pragma GCC diagnostic pop
#endif /*SSCE_FALLOC_H*/