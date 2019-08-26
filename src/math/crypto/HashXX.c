#include "Hash.h"

#include <Macros.h>

#include <stdint.h>
#include <stdlib.h>

/*
 * This is a reduced version of xxHash v0.7.1 found at:
 * https://github.com/Cyan4973/xxHash/releases/tag/v0.7.1
 */

/*
 *  xxHash - Fast Hash algorithm
 *  Copyright (C) 2012-2016, Yann Collet
 *
 *  BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You can contact the author at :
 *  - xxHash homepage: http://www.xxhash.com
 *  - xxHash source repository : https://github.com/Cyan4973/xxHash
 */

#if IS_ARM && !__ARM_FEATURE_UNALIGNED
  #define XXH_FORCE_ALIGN_CHECK 1
#else
  #define XXH_FORCE_ALIGN_CHECK 0
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define XXH_CPU_LITTLE_ENDIAN 1
#else
  #define XXH_CPU_LITTLE_ENDIAN 0
#endif

#define XXH_FORCE_MEMORY_ACCESS !XXH_FORCE_ALIGN_CHECK

typedef enum {
  XXH_ALIGNED,
  XXH_UNALIGNED
} XXHAlignment;

static const uint32_t PRIME32_1 = 0x9E3779B1U;   /* 0b10011110001101110111100110110001 */
static const uint32_t PRIME32_2 = 0x85EBCA77U;   /* 0b10000101111010111100101001110111 */
static const uint32_t PRIME32_3 = 0xC2B2AE3DU;   /* 0b11000010101100101010111000111101 */
static const uint32_t PRIME32_4 = 0x27D4EB2FU;   /* 0b00100111110101001110101100101111 */
static const uint32_t PRIME32_5 = 0x165667B1U;   /* 0b00010110010101100110011110110001 */

#if defined(XXH_FORCE_MEMORY_ACCESS) && XXH_FORCE_MEMORY_ACCESS
  /*
   * Force direct memory access.
   * Only works on CPU which support unaligned memory access in hardware.
   */
  static inline uint32_t internal_xxh_read32(const void* memPtr) {
    return *(const uint32_t*)memPtr;
  }
#else
  /*
   * Portable and safe solution.
   * Generally efficient.
   * See: http://stackoverflow.com/a/32095106/646947
   */
  static inline uint32_t internal_xxh_read32(const void* memPtr) {
    uint32_t val;
    __builtin_memcpy(&val, memPtr, sizeof(val));
    return val;
  }
#endif

#define internal_xxh_swap32 __builtin_bswap32

static inline uint32_t internal_xxh_readLE32(const void* ptr) {
  return XXH_CPU_LITTLE_ENDIAN ? internal_xxh_read32(ptr) : internal_xxh_swap32(internal_xxh_read32(ptr));
}

static inline uint32_t internal_xxh_readLE32_align(const void* ptr, XXHAlignment align) {
  if(align == XXH_UNALIGNED) {
    return internal_xxh_readLE32(ptr);
  } else {
    return XXH_CPU_LITTLE_ENDIAN ? *(const uint32_t*)ptr : internal_xxh_swap32(*(const uint32_t*)ptr);
  }
}

#define internal_xxh_get32bits(p) internal_xxh_readLE32_align(p, align)

#define internal_xxh_rotl32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))

static inline uint32_t internal_xxh32_round(uint32_t acc, uint32_t input) {
  acc += input * PRIME32_2;
  acc = internal_xxh_rotl32(acc, 13);
  acc *= PRIME32_1;
  return acc;
}

/* mix all bits */
static inline uint32_t internal_xxh32_avalanche(uint32_t h32) {
  h32 ^= h32 >> 15;
  h32 *= PRIME32_2;
  h32 ^= h32 >> 13;
  h32 *= PRIME32_3;
  h32 ^= h32 >> 16;
  return h32;
}

static inline uint32_t internal_xxh32_finalize(uint32_t h32, const void* ptr, size_t len, XXHAlignment align) {
  const uint8_t* p = (const uint8_t*)ptr;

  #define PROCESS1             \
    h32 += (*p++) * PRIME32_5; \
    h32 = internal_xxh_rotl32(h32, 11) * PRIME32_1;

  #define PROCESS4                                \
    h32 += internal_xxh_get32bits(p) * PRIME32_3; \
    p += 4;                                       \
    h32 = internal_xxh_rotl32(h32, 17) * PRIME32_4;

  // or switch(bEnd - p)
  switch(len & 15) {
    case 12:
      PROCESS4;
      /* fallthrough */
    case 8:
      PROCESS4;
      /* fallthrough */
    case 4:
      PROCESS4;
      return internal_xxh32_avalanche(h32);

    case 13:
      PROCESS4;
      /* fallthrough */
    case 9:
      PROCESS4;
      /* fallthrough */
    case 5:
      PROCESS4;
      PROCESS1;
      return internal_xxh32_avalanche(h32);

    case 14:
      PROCESS4;
      /* fallthrough */
    case 10:
      PROCESS4;
      /* fallthrough */
    case 6:
      PROCESS4;
      PROCESS1;
      PROCESS1;
      return internal_xxh32_avalanche(h32);

    case 15:
      PROCESS4;
      /* fallthrough */
    case 11:
      PROCESS4;
      /* fallthrough */
    case 7:
      PROCESS4;
      /* fallthrough */
    case 3:
      PROCESS1;
      /* fallthrough */
    case 2:
      PROCESS1;
      /* fallthrough */
    case 1:
      PROCESS1;
      /* fallthrough */
    case 0:
      return internal_xxh32_avalanche(h32);
  }
  EARLY_TRACE("internal_xxh32_finalize unreachables' reached!");
  abort();
  /* reaching this point is deemed impossible */
}

static inline uint32_t internal_xxh32_endian_align(const void* input, size_t len, uint32_t seed, XXHAlignment align) {
  const uint8_t* p = (const uint8_t*)input;
  const uint8_t* bEnd = p + len;
  uint32_t h32;

  if(len >= 16) {
    const uint8_t* const limit = bEnd - 15;
    uint32_t v1 = seed + PRIME32_1 + PRIME32_2;
    uint32_t v2 = seed + PRIME32_2;
    uint32_t v3 = seed + 0;
    uint32_t v4 = seed - PRIME32_1;

    do {
      v1 = internal_xxh32_round(v1, internal_xxh_get32bits(p));
      p += 4;
      v2 = internal_xxh32_round(v2, internal_xxh_get32bits(p));
      p += 4;
      v3 = internal_xxh32_round(v3, internal_xxh_get32bits(p));
      p += 4;
      v4 = internal_xxh32_round(v4, internal_xxh_get32bits(p));
      p += 4;
    } while(p < limit);

    h32 = internal_xxh_rotl32(v1, 1) + internal_xxh_rotl32(v2, 7) + internal_xxh_rotl32(v3, 12) + internal_xxh_rotl32(v4, 18);
  } else {
    h32 = seed + PRIME32_5;
  }

  h32 += (uint32_t)len;

  return internal_xxh32_finalize(h32, p, len & 15, align);
}

uint32_t ncrypto_xxhash32(const void* input, size_t len, uint32_t seed) {
  #if defined(XXH_FORCE_ALIGN_CHECK) && XXH_FORCE_ALIGN_CHECK
    if((((uintptr_t)input) & 3) == 0) {
      /* Input is 4-uint8_ts aligned, leverage the speed benefit */
      return internal_xxh32_endian_align(input, len, seed, XXH_ALIGNED);
    }
  #endif
  return internal_xxh32_endian_align(input, len, seed, XXH_UNALIGNED);
}