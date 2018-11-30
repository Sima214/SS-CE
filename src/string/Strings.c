#include "Strings.h"

#include <Macros.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

String multi_concat(const size_t count, ...) {
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

#define bulk_swap(dst, src, len, type) \
  while(len >= sizeof(type)) {         \
    len -= sizeof(type);               \
    type* dstp = dst;                  \
    type* srcp = src;                  \
    type tmp0 = *dstp;                 \
    type tmp1 = *srcp;                 \
    *srcp = tmp0;                      \
    *dstp = tmp1;                      \
    dst = (void*)(dstp + 1);           \
    src = (void*)(srcp + 1);           \
  }

#define single_swap(dst, src, len, type) \
  if((len / sizeof(type)) == 1) {        \
    len -= sizeof(type);                 \
    type* dstp = dst;                    \
    type* srcp = src;                    \
    type tmp0 = *dstp;                   \
    type tmp1 = *srcp;                   \
    *srcp = tmp0;                        \
    *dstp = tmp1;                        \
    dst = (void*)(dstp + 1);             \
    src = (void*)(srcp + 1);             \
  }

MARK_UNUSED
static void memswap_generic32(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, uint32_t);
  single_swap(dst, src, len, uint16_t);
  single_swap(dst, src, len, uint8_t);
}

MARK_UNUSED
static void memswap_generic64(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, uint64_t);
  single_swap(dst, src, len, uint32_t);
  single_swap(dst, src, len, uint16_t);
  single_swap(dst, src, len, uint8_t);
}

__attribute__((__target__("sse2"), optimize("no-tree-vectorize"))) static void memswap_sse2(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m128i_u);
  single_swap(dst, src, len, uint64_t);
  single_swap(dst, src, len, uint32_t);
  single_swap(dst, src, len, uint16_t);
  single_swap(dst, src, len, uint8_t);
}

__attribute__((__target__("avx"), optimize("no-tree-vectorize"))) static void memswap_avx(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m256i_u);
  single_swap(dst, src, len, __m128i_u);
  single_swap(dst, src, len, uint64_t);
  single_swap(dst, src, len, uint32_t);
  single_swap(dst, src, len, uint16_t);
  single_swap(dst, src, len, uint8_t);
}

static void* resolve_memswap() {
  #if defined(__x86_64__)
  cpu_init();
  if(__builtin_cpu_supports("avx")) {
      #ifndef NDEBUG
    native_puts("Selecting memswap_avx");
      #endif
    return memswap_avx;
  } else {
// x86_64 always supports SSE2
      #ifndef NDEBUG
    native_puts("Selecting memswap_sse");
      #endif
    return memswap_sse2;
  }
  #elif defined(__i386__)
  __builtin_cpu_init();
  if(__builtin_cpu_supports("avx")) {
      #ifndef NDEBUG
    native_puts("Selecting memswap_avx");
      #endif
    return memswap_avx;
  } else if(__builtin_cpu_supports("sse2")) {
      #ifndef NDEBUG
    native_puts("Selecting memswap_sse2");
      #endif
    return memswap_sse2;
  } else {
      #ifndef NDEBUG
    native_puts("Selecting memswap_generic32");
      #endif
    return memswap_generic32;
  }
  #else
    #error Unsupported cpu architecture!
  #endif
}

void memswap(void*, void*, size_t)
  __attribute__((ifunc("resolve_memswap")));