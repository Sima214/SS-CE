#include "Memory.h"
#include "Swap.h"

#include <Macros.h>
#include <Runtime.h>

#include <stddef.h>
#include <stdint.h>
#include <x86intrin.h>

TARGET_EXT(sse2) static void memswap_sse2(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

TARGET_EXT(avx) static void memswap_avx(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m256i, _mm256_loadu_si256, _mm256_storeu_si256);
  _mm256_zeroupper();
  single_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

TARGET_EXT(avx512f) static void memswap_avx512(void* dst, void* src, size_t len) {
  bulk_swap(dst, src, len, __m512i, _mm512_loadu_si512, _mm512_storeu_si512);
  single_swap(dst, src, len, __m256i, _mm256_loadu_si256, _mm256_storeu_si256);
  _mm256_zeroupper();
  single_swap(dst, src, len, __m128i, _mm_loadu_si128, _mm_storeu_si128);
  single_swap(dst, src, len, uint64_t, load_uint64_t, store_uint64_t);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}

MARK_COLD static memswap_t* resolve_memswap() {
  Runtime* features = ssce_get_runtime();
  if(features->cpu_x86_avx512f) {
    EARLY_TRACE("Selecting memswap_avx512");
    return memswap_avx512;
  } else if(features->cpu_x86_avx) {
    EARLY_TRACE("Selecting memswap_avx");
    return memswap_avx;
  } else {
    // x86_64 always supports SSE2
    EARLY_TRACE("Selecting memswap_sse");
    return memswap_sse2;
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