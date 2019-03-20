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
 * Special pointer of type \ref type, with some special properties.
 * This must be used before the type of any pointer allocated with fast_malloc!
 */
#define falloc_ptr(type) __attribute__((cleanup(falloc_free_auto))) type* const

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
 * Allocates \ref l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned sizeof(int) bytes.
 * If not enough memory could be allocated, then NULL is returned.
 */
#define falloc_malloc(l) falloc_malloc_aligned(l, sizeof(int))

/**
 * Allocates \ref l bytes and returns
 * a pointer to the start of the allocated memory.
 * The returned pointer is aligned \ref align bytes.
 * If not enough memory could be allocated, then NULL is returned.
 */
MARK_UNUSED FORCE_INLINE MARK_MALLOC(2, 1) static void* falloc_malloc_aligned(size_t l, size_t align) {
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
FORCE_INLINE static void falloc_free(void* ptr) {
  // Null pointer check.
  if(ptr == NULL) {
    EARLY_TRACE("falloc_free got a null pointer!");
    return;
  }
  ThreadLocalStack* tls = falloc_get_tls();
  // In debug build check bounds.
  uintptr_t start = (uintptr_t)tls->start;
  uintptr_t end = start + tls->usage;
  uintptr_t ptri = (uintptr_t)ptr;
  if(COLD_BRANCH(ptri < start || ptri >= end)) {
    EARLY_TRACE("falloc_free got a pointer which is out of bounds!");
    return;
  }
  // Make new end be the pointer which gets deallocated.
  size_t ptr_len = end - ptri;
  tls->usage -= ptr_len;
  #ifndef NDEBUG
    uintptr_t new_end = start + tls->usage;
    if(new_end != ptri) {
      EARLY_TRACE("New end is not pointer getting deallocated!");
      abort();
    }
  #endif
}

/**
 * Takes a pointer to the pointer allocated by \ref falloc_malloc_aligned and deallocates it.
 * This will be called automatically on function exit if the \ref falloc_ptr is used.
 */
MARK_UNUSED FORCE_INLINE static void falloc_free_auto(void* pptr) {
  void* ptr = *(void**)(pptr);
  falloc_free(ptr);
}

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