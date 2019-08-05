#include "Memory.h"
#include "Swap.h"

#include <stddef.h>
#include <stdint.h>
#include <arm_neon.h>

void memswap(void* dst, void* src, size_t len) {
  bulk_swap2(dst, src, len, uint64x2_t, uint64_t, vld1q_u64, vst1q_u64);
  single_swap(dst, src, len, uint64_t, vld1_u64, vst1_u64);
  single_swap(dst, src, len, uint32_t, load_uint32_t, store_uint32_t);
  single_swap(dst, src, len, uint16_t, load_uint16_t, store_uint16_t);
  single_swap(dst, src, len, uint8_t, load_uint8_t, store_uint8_t);
}