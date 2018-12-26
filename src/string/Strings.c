#include "Strings.h"

#include <Macros.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

typedef void(memswap_t)(void*, void*, size_t);

EXPORT_API String multi_concat(const size_t count, ...) {
  String cache[count];
  String final = {NULL, 0};
  //Prepare vargs.
  va_list vargs;
  va_start(vargs, count);
  for(size_t i = 0; i < count; i++) {
    cache[i] = va_arg(vargs, String);
    final.len += cache[i].len;
  }
  va_end(vargs);
  final.array = malloc(final.len + 1);
  if(final.array == NULL) {
    return (String){NULL, 0};
  }
  char* sp = final.array;
  for(size_t i = 0; i < count; i++) {
    String cur = cache[i];
    memcpy(sp, cur.array, cur.len);
    sp += cur.len;
  }
  *(sp) = '\0';
  return final;
}

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
    block_swap(dst, src, len, type, load_func, store_func)    \
  }

#define single_swap(dst, src, len, type, load_func, store_func) \
  if((len / sizeof(type)) == 1) {                               \
    block_swap(dst, src, len, type, load_func, store_func)      \
  }

MARK_UNUSED
static void memswap_generic32(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

MARK_UNUSED
static void memswap_generic64(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

__attribute__((__target__("sse2"), optimize("no-tree-vectorize")))
static void memswap_sse2(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

__attribute__((__target__("avx"), optimize("no-tree-vectorize")))
static void memswap_avx(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m256i, _mm256_loadu_si256, _mm256_storeu_si256);
  single_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

__attribute__((__target__("avx512f"), optimize("no-tree-vectorize")))
static void memswap_avx512(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m512i, _mm512_loadu_si512, _mm512_storeu_si512);
  single_swap(dst, src, len, __m256i, _mm256_loadu_si256, _mm256_storeu_si256);
  single_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

static memswap_t* resolve_memswap() {
  #if defined(__x86_64__)
    cpu_init();
    if(__builtin_cpu_supports("avx512f")) {
      #ifndef NDEBUG
        native_puts("Selecting memswap_avx512");
      #endif
      return memswap_avx512;
    }
    else if(__builtin_cpu_supports("avx")) {
      #ifndef NDEBUG
        native_puts("Selecting memswap_avx");
      #endif
      return memswap_avx;
    }
    else {
      // x86_64 always supports SSE2
      #ifndef NDEBUG
        native_puts("Selecting memswap_sse");
      #endif
      return memswap_sse2;
    }
  #elif defined(__i386__)
  cpu_init();
  if(__builtin_cpu_supports("avx")) {
      #ifndef NDEBUG
        native_puts("Selecting memswap_avx");
      #endif
    return memswap_avx;
  }
  else if(__builtin_cpu_supports("sse2")) {
      #ifndef NDEBUG
        native_puts("Selecting memswap_sse2");
      #endif
    return memswap_sse2;
  }
  else {
      #ifndef NDEBUG
        native_puts("Selecting memswap_generic32");
      #endif
    return memswap_generic32;
  }
  #else
    #error Unsupported cpu architecture!
  #endif
}

EXPORT_API_RUNTIME(resolve_memswap) void memswap(void*, void*, size_t);