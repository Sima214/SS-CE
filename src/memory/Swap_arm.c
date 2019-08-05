#include "Memory.h"
#include "Swap.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __ARM_NEON__
    #include <arm_neon.h>
#endif

#ifndef __ARM_NEON__
  void memswap(void* dst, void* src, size_t len) {
    bulk_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
    single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
    single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
  }
#else
  void memswap(void* dst, void* src, size_t len) {
    bulk_swap(dst, src, len, uint64_t, vld1_u64, vst1_u64);
    single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
    single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
    single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
  }
#endif