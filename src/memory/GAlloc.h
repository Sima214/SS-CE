#ifndef SSCE_GALLOC_H
#define SSCE_GALLOC_H
/**
 * @file
 * @brief This file provides a generic malloc.
 * This is usually provided jemalloc, but this may change in the future.
 */

#include <Macros.h>

// Include std malloc, so we can later override them and not cause any problems.
#include <stddef.h>
#include <stdlib.h>

/**
 * Your usual malloc.
 */
EXPORT_API void* galloc_malloc(size_t size) MARK_MALLOC(1);

/**
 * Your usual calloc.
 */
EXPORT_API void* galloc_calloc(size_t num, size_t size) MARK_MALLOC(1, 2);

/**
 * Your usual realloc.
 */
EXPORT_API void* galloc_realloc(void *ptr, size_t size) MARK_MALLOC(2);

/**
 * Your usual free.
 */
EXPORT_API void galloc_free(void *ptr);

/**
 * Returns memory usage in bytes of pointer \p ptr.
 */
EXPORT_API size_t galloc_size(void *ptr);

/**
 * Dumps allocator statistics at stdout.
 */
EXPORT_API void galloc_dump_stats();

#ifndef GALLOC_IMPLEMENTATION
  #define malloc galloc_malloc
  #define calloc galloc_calloc
  #define realloc galloc_realloc
  #define free galloc_free
#endif

#endif /*SSCE_GALLOC_H*/