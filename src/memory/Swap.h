#ifndef SSCE_SWAP
#define SSCE_SWAP
/**
* @file
* @brief Common code for memswap.
*/

#include <stdint.h>

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

#define block_swap2(dst, src, len, type, type2, load_func, store_func) \
  len -= sizeof(type);                                                 \
  type2* dstp = dst;                                                   \
  type2* srcp = src;                                                   \
  type tmp0 = load_func(dstp);                                         \
  type tmp1 = load_func(srcp);                                         \
  store_func(srcp, tmp0);                                              \
  store_func(dstp, tmp1);                                              \
  dst = (void*)(dstp + 1);                                             \
  src = (void*)(srcp + 1);

#define bulk_swap2(dst, src, len, type, type2, load_func, store_func) \
  while(len >= sizeof(type)) {                                        \
    block_swap2(dst, src, len, type, type2, load_func, store_func);   \
  }

typedef void(memswap_t)(void*, void*, size_t);

#endif /*SSCE_SWAP*/