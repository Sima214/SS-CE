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
 * Special pointer of type \ref type, with some special properties.
 * This must be used before the type of any pointer allocated with fast_malloc!
 */
#define fast_ptr(type) __attribute__((cleanup(falloc_free))) type* const

/**
 * Allocates \ref l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned sizeof(int) bytes.
 * If not enough memory could be allocated, then NULL is returned.
 */
#define fast_malloc(l) falloc_malloc_aligned(l, sizeof(int))

/**
 * Allocates \ref l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned \ref align bytes.
 * If not enough memory could be allocated, then NULL is returned.
 */
MARK_MALLOC(2, 1) static inline void* falloc_malloc_aligned(size_t l, size_t align) {
  // Calculate optimal start address.
  return NULL;
}

/**
 * Takes a pointer to the pointer allocated by \ref fast_malloc.
 * This is called automatically on function exit if the fast_t is used.
 */
static inline void falloc_free(void** pptr) {
  void* ptr = *pptr;
}

/**
 * Internal api usage only!
 * Returns info about allocated thread local stack.
 */
EXPORT_API ThreadLocalStack* falloc_get_tls();

/**
 * Internal api usage only!
 * Ensures that there are at least \ref l free in \ref tls.
 * Returns non-zero value on error.
 */
EXPORT_API int falloc_ensure_space(ThreadLocalStack* tls, size_t l);

/**
 * Initializes thread local keys.
 */
void internal_falloc_init();

/**
 * Cleans up thread local keys.
 */
void internal_falloc_exit();

#endif /*SSCE_FALLOC_H*/