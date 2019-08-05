#include "Memory.h"
#include "Swap.h"

#include <Macros.h>
#include <Runtime.h>

#include <stddef.h>
#include <stdint.h>
#include <x86intrin.h>

typedef void(memswap_t)(void*, void*, size_t);

/*
 * Generate unaligned load/store functions to feed the swap macros.
 */
#define GENERATE_DEFAULT_LOAD_STORE(type)              \
  static inline type load_##type(type* src) {          \
    return *src;                                       \
  }                                                    \
  static inline void store_##type(type* dst, type v) { \
    *dst = v;                                          \
  }

GENERATE_DEFAULT_LOAD_STORE(uint8_t);
GENERATE_DEFAULT_LOAD_STORE(uint16_t);
GENERATE_DEFAULT_LOAD_STORE(uint32_t);
GENERATE_DEFAULT_LOAD_STORE(uint64_t);

#define block_swap(dst, src, len, type, load_func, store_func) \
  len -= sizeof(type);                                         \
  type* dstp = dst;                                            \
  type* srcp = src;                                            \
  type tmp0 = load_func(dstp);                                 \
  type tmp1 = load_func(srcp);                                 \
  store_func(srcp, tmp0);                                      \
  store_func(dstp, tmp1);                                      \
  dst = (void*)(dstp + 1);                                     \
  src = (void*)(srcp + 1);

#define bulk_swap(dst, src, len, type, load_func, store_func) \
  while(len >= sizeof(type)) {                                \
    block_swap(dst, src, len, type, load_func, store_func);   \
  }

#define single_swap(dst, src, len, type, load_func, store_func) \
  if((len / sizeof(type)) == 1) {                               \
    block_swap(dst, src, len, type, load_func, store_func);     \
  }

static void memswap_generic32(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

TARGET_EXT(sse2) static void memswap_sse2(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

static memswap_t* resolve_memswap() {
  Runtime* features = ssce_get_runtime();
  if(features->cpu_x86_sse2) {
    EARLY_TRACE("Selecting memswap_sse2");
    return memswap_sse2;
  }
  else {
    EARLY_TRACE("Selecting memswap_generic32");
    return memswap_generic32;
  }
}

#if defined(LINK_STATIC)
  void memswap(void* dst, void* src, size_t len) {
    static memswap_t* resolved = NULL;
    if(resolved == NULL) {
      resolved = resolve_memswap();
    }
    (*resolved)(dst, src, len);
  }
#elif defined(LINK_ELF)
  EXPORT_API_RUNTIME(resolve_memswap) void memswap(void*, void*, size_t);
#elif defined(LINK_MACHO)
  // TODO: replace with macho symbol resolvers?
  void memswap(void* dst, void* src, size_t len) {
    static memswap_t* resolved = NULL;
    if(resolved == NULL) {
      resolved = resolve_memswap();
    }
    (*resolved)(dst, src, len);
  }
#elif defined(LINK_PE)
  void memswap(void* dst, void* src, size_t len) {
    static memswap_t* resolved = NULL;
    // TODO: patch all the IATs.
    if(resolved == NULL) {
      resolved = resolve_memswap();
    }
    (*resolved)(dst, src, len);
  }
#else
  #error Unsupported link format!
#endif