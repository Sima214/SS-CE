#define GALLOC_IMPLEMENTATION
#define JEMALLOC_NO_RENAME

#include "GAlloc.h"

#include <Config.h>
#include <Logger.h>

#include <jemalloc.h>

#include <stdlib.h>

/**
 * Proxy calls to jemalloc.
 */

void* galloc_malloc(size_t size) {
  return je_malloc(size);
}

void* galloc_calloc(size_t num, size_t size) {
  return je_calloc(num, size);
}

void* galloc_realloc(void* ptr, size_t size) {
  return je_realloc(ptr, size);
}

void galloc_free(void* ptr) {
  return je_free(ptr);
}

size_t galloc_size(void* ptr) {
  return je_sallocx(ptr, 0);
}

void galloc_dump_stats() {
  return je_malloc_stats_print(NULL, NULL, MEMORY_GALLOC_DUMP_OPTIONS);
}

#ifdef MEMORY_GALLOC_OVERRIDE
  /**
   * Override std symbols of platform supports weak symbols.
   * Could also use MARK_ALIAS, but requires getting the signature just perfect.
   */
  EXPORT_API void* malloc(size_t size) {
    return galloc_malloc(size);
  }

  EXPORT_API void* calloc(size_t num, size_t size) {
    return galloc_calloc(num, size);
  }

  EXPORT_API void* realloc(void* ptr, size_t size) {
    return galloc_realloc(ptr, size);
  }

  EXPORT_API void free(void* ptr) {
    return galloc_free(ptr);
  }
#endif