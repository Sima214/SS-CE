#include "Hash.h"

#include <BiggerNumbers.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*
 * This is a SpookyHash implementation ported in C.
 * Original sources can be found at:
 * http://burtleburtle.net/bob/hash/spooky.html
 */

/*
 * Spooky Hash
 * A 128-bit noncryptographic hash, for checksums and table lookup
 * By Bob Jenkins.  Public domain.
 *   Oct 31 2010: published framework, disclaimer ShortHash isn't right
 *   Nov 7 2010: disabled ShortHash
 *   Oct 31 2011: replace End, internal_spooky_hash_short_mix, ShortEnd, enable ShortHash again
 *   April 10 2012: buffer overflow on platforms without unaligned reads
 *   July 12 2012: was passing out variables in final to in/out in short
 *   July 30 2012: I reintroduced the buffer overflow
 *   August 5 2012: SpookyV2: d = should be d += in short hash, and remove extra mix from long hash
 */
#if IS_ARM && !__ARM_FEATURE_UNALIGNED
  #define ALLOW_UNALIGNED_READS 0
#else
  #define ALLOW_UNALIGNED_READS 1
#endif
// number of uint64's in internal state
#define NUM_VARS 12
// size of the internal state
#define BLOCK_SIZE (NUM_VARS * 8)
// size of buffer of unhashed data, in bytes
#define BUF_SIZE (2 * BLOCK_SIZE)
/*
 * sc_const: a constant which:
 * - is not zero
 * - is odd
 * - is a not-very-regular mix of 1's and 0's
 * - does not need any other special mathematical properties
 */
static const uint64_t sc_const = 0xdeadbeefdeadbeefLL;

/*
 * left rotate a 64-bit value by k bytes
 */
#define internal_spooky_hash_rot64(x, k) ((x << k) | (x >> (64 - k)))

/*
 * Mix all 4 inputs together so that h0, h1 are a hash of them all.
 *
 * For two inputs differing in just the input bits
 * Where "differ" means xor or subtraction
 * And the base value is random, or a counting value starting at that bit
 * The final result will have each bit of h0, h1 flip
 * For every input bit,
 * with probability 50 +- .3% (it is probably better than that)
 * For every pair of input bits,
 * with probability 50 +- .75% (the worst case is approximately that).
 */
#define internal_spooky_hash_short_end(h0, h1, h2, h3) \
  h3 ^= h2;                                            \
  h2 = internal_spooky_hash_rot64(h2, 15);             \
  h3 += h2;                                            \
  h0 ^= h3;                                            \
  h3 = internal_spooky_hash_rot64(h3, 52);             \
  h0 += h3;                                            \
  h1 ^= h0;                                            \
  h0 = internal_spooky_hash_rot64(h0, 26);             \
  h1 += h0;                                            \
  h2 ^= h1;                                            \
  h1 = internal_spooky_hash_rot64(h1, 51);             \
  h2 += h1;                                            \
  h3 ^= h2;                                            \
  h2 = internal_spooky_hash_rot64(h2, 28);             \
  h3 += h2;                                            \
  h0 ^= h3;                                            \
  h3 = internal_spooky_hash_rot64(h3, 9);              \
  h0 += h3;                                            \
  h1 ^= h0;                                            \
  h0 = internal_spooky_hash_rot64(h0, 47);             \
  h1 += h0;                                            \
  h2 ^= h1;                                            \
  h1 = internal_spooky_hash_rot64(h1, 54);             \
  h2 += h1;                                            \
  h3 ^= h2;                                            \
  h2 = internal_spooky_hash_rot64(h2, 32);             \
  h3 += h2;                                            \
  h0 ^= h3;                                            \
  h3 = internal_spooky_hash_rot64(h3, 25);             \
  h0 += h3;                                            \
  h1 ^= h0;                                            \
  h0 = internal_spooky_hash_rot64(h0, 63);             \
  h1 += h0;

/**
 * The goal is for each bit of the input to expand into 128 bits of
 * apparent entropy before it is fully overwritten.
 * n trials both set and cleared at least m bits of h0 h1 h2 h3
 *   n: 2   m: 29
 *   n: 3   m: 46
 *   n: 4   m: 57
 *   n: 5   m: 107
 *   n: 6   m: 146
 *   n: 7   m: 152
 * when run forwards or backwards
 * for all 1-bit and 2-bit diffs
 * with diffs defined by either xor or subtraction
 * with a base of all zeros plus a counter, or plus another bit, or random
 */
#define internal_spooky_hash_short_mix(h0, h1, h2, h3) \
  h2 = internal_spooky_hash_rot64(h2, 50);             \
  h2 += h3;                                            \
  h0 ^= h2;                                            \
  h3 = internal_spooky_hash_rot64(h3, 52);             \
  h3 += h0;                                            \
  h1 ^= h3;                                            \
  h0 = internal_spooky_hash_rot64(h0, 30);             \
  h0 += h1;                                            \
  h2 ^= h0;                                            \
  h1 = internal_spooky_hash_rot64(h1, 41);             \
  h1 += h2;                                            \
  h3 ^= h1;                                            \
  h2 = internal_spooky_hash_rot64(h2, 54);             \
  h2 += h3;                                            \
  h0 ^= h2;                                            \
  h3 = internal_spooky_hash_rot64(h3, 48);             \
  h3 += h0;                                            \
  h1 ^= h3;                                            \
  h0 = internal_spooky_hash_rot64(h0, 38);             \
  h0 += h1;                                            \
  h2 ^= h0;                                            \
  h1 = internal_spooky_hash_rot64(h1, 37);             \
  h1 += h2;                                            \
  h3 ^= h1;                                            \
  h2 = internal_spooky_hash_rot64(h2, 62);             \
  h2 += h3;                                            \
  h0 ^= h2;                                            \
  h3 = internal_spooky_hash_rot64(h3, 34);             \
  h3 += h0;                                            \
  h1 ^= h3;                                            \
  h0 = internal_spooky_hash_rot64(h0, 5);              \
  h0 += h1;                                            \
  h2 ^= h0;                                            \
  h1 = internal_spooky_hash_rot64(h1, 36);             \
  h1 += h2;                                            \
  h3 ^= h1;

//
// This is used if the input is 96 bytes long or longer.
//
// The internal state is fully overwritten every 96 bytes.
// Every input bit appears to cause at least 128 bits of entropy
// before 96 other bytes are combined, when run forward or backward
//   For every input bit,
//   Two inputs differing in just that input bit
//   Where "differ" means xor or subtraction
//   And the base value is random
//   When run forward or backwards one Mix
// I tried 3 pairs of each; they all differed by at least 212 bits.
//
#define internal_spooky_hash_mix(data, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11) \
  s0 += data[0];                                                                         \
  s2 ^= s10;                                                                             \
  s11 ^= s0;                                                                             \
  s0 = internal_spooky_hash_rot64(s0, 11);                                               \
  s11 += s1;                                                                             \
  s1 += data[1];                                                                         \
  s3 ^= s11;                                                                             \
  s0 ^= s1;                                                                              \
  s1 = internal_spooky_hash_rot64(s1, 32);                                               \
  s0 += s2;                                                                              \
  s2 += data[2];                                                                         \
  s4 ^= s0;                                                                              \
  s1 ^= s2;                                                                              \
  s2 = internal_spooky_hash_rot64(s2, 43);                                               \
  s1 += s3;                                                                              \
  s3 += data[3];                                                                         \
  s5 ^= s1;                                                                              \
  s2 ^= s3;                                                                              \
  s3 = internal_spooky_hash_rot64(s3, 31);                                               \
  s2 += s4;                                                                              \
  s4 += data[4];                                                                         \
  s6 ^= s2;                                                                              \
  s3 ^= s4;                                                                              \
  s4 = internal_spooky_hash_rot64(s4, 17);                                               \
  s3 += s5;                                                                              \
  s5 += data[5];                                                                         \
  s7 ^= s3;                                                                              \
  s4 ^= s5;                                                                              \
  s5 = internal_spooky_hash_rot64(s5, 28);                                               \
  s4 += s6;                                                                              \
  s6 += data[6];                                                                         \
  s8 ^= s4;                                                                              \
  s5 ^= s6;                                                                              \
  s6 = internal_spooky_hash_rot64(s6, 39);                                               \
  s5 += s7;                                                                              \
  s7 += data[7];                                                                         \
  s9 ^= s5;                                                                              \
  s6 ^= s7;                                                                              \
  s7 = internal_spooky_hash_rot64(s7, 57);                                               \
  s6 += s8;                                                                              \
  s8 += data[8];                                                                         \
  s10 ^= s6;                                                                             \
  s7 ^= s8;                                                                              \
  s8 = internal_spooky_hash_rot64(s8, 55);                                               \
  s7 += s9;                                                                              \
  s9 += data[9];                                                                         \
  s11 ^= s7;                                                                             \
  s8 ^= s9;                                                                              \
  s9 = internal_spooky_hash_rot64(s9, 54);                                               \
  s8 += s10;                                                                             \
  s10 += data[10];                                                                       \
  s0 ^= s8;                                                                              \
  s9 ^= s10;                                                                             \
  s10 = internal_spooky_hash_rot64(s10, 22);                                             \
  s9 += s11;                                                                             \
  s11 += data[11];                                                                       \
  s1 ^= s9;                                                                              \
  s10 ^= s11;                                                                            \
  s11 = internal_spooky_hash_rot64(s11, 46);                                             \
  s10 += s0;

/*
 * Mix all 12 inputs together so that h0, h1 are a hash of them all.
 *
 * For two inputs differing in just the input bits
 * Where "differ" means xor or subtraction
 * And the base value is random, or a counting value starting at that bit
 * The final result will have each bit of h0, h1 flip
 * For every input bit,
 * with probability 50 +- .3%
 * For every pair of input bits,
 * with probability 50 +- 3%
 *
 * This does not rely on the last Mix() call having already mixed some.
 * Two iterations was almost good enough for a 64-bit result, but a
 * 128-bit result is reported, so End() does three iterations.
 */
#define internal_spooky_hash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11) \
  h11 += h1;                                                                               \
  h2 ^= h11;                                                                               \
  h1 = internal_spooky_hash_rot64(h1, 44);                                                 \
  h0 += h2;                                                                                \
  h3 ^= h0;                                                                                \
  h2 = internal_spooky_hash_rot64(h2, 15);                                                 \
  h1 += h3;                                                                                \
  h4 ^= h1;                                                                                \
  h3 = internal_spooky_hash_rot64(h3, 34);                                                 \
  h2 += h4;                                                                                \
  h5 ^= h2;                                                                                \
  h4 = internal_spooky_hash_rot64(h4, 21);                                                 \
  h3 += h5;                                                                                \
  h6 ^= h3;                                                                                \
  h5 = internal_spooky_hash_rot64(h5, 38);                                                 \
  h4 += h6;                                                                                \
  h7 ^= h4;                                                                                \
  h6 = internal_spooky_hash_rot64(h6, 33);                                                 \
  h5 += h7;                                                                                \
  h8 ^= h5;                                                                                \
  h7 = internal_spooky_hash_rot64(h7, 10);                                                 \
  h6 += h8;                                                                                \
  h9 ^= h6;                                                                                \
  h8 = internal_spooky_hash_rot64(h8, 13);                                                 \
  h7 += h9;                                                                                \
  h10 ^= h7;                                                                               \
  h9 = internal_spooky_hash_rot64(h9, 38);                                                 \
  h8 += h10;                                                                               \
  h11 ^= h8;                                                                               \
  h10 = internal_spooky_hash_rot64(h10, 53);                                               \
  h9 += h11;                                                                               \
  h0 ^= h9;                                                                                \
  h11 = internal_spooky_hash_rot64(h11, 42);                                               \
  h10 += h0;                                                                               \
  h1 ^= h10;                                                                               \
  h0 = internal_spooky_hash_rot64(h0, 54);

#define internal_spooky_hash_end(data, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11) \
  h0 += data[0];                                                                         \
  h1 += data[1];                                                                         \
  h2 += data[2];                                                                         \
  h3 += data[3];                                                                         \
  h4 += data[4];                                                                         \
  h5 += data[5];                                                                         \
  h6 += data[6];                                                                         \
  h7 += data[7];                                                                         \
  h8 += data[8];                                                                         \
  h9 += data[9];                                                                         \
  h10 += data[10];                                                                       \
  h11 += data[11];                                                                       \
  internal_spooky_hash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);    \
  internal_spooky_hash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);    \
  internal_spooky_hash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);

static void internal_spooky_hash_short(const void* message, size_t length, uint64_t* hash1, uint64_t* hash2) {
  uint64_t buf[2 * NUM_VARS];
  union {
    const uint8_t* p8;
    uint32_t* p32;
    uint64_t* p64;
    size_t i;
  } u;

  u.p8 = (const uint8_t* )message;

  if(!ALLOW_UNALIGNED_READS && (u.i & 0x7)) {
    __builtin_memcpy(buf, message, length);
    u.p64 = buf;
  }

  size_t remainder = length % 32;
  uint64_t a = *hash1;
  uint64_t b = *hash2;
  uint64_t c = sc_const;
  uint64_t d = sc_const;

  if(length > 15) {
    const uint64_t* end = u.p64 + (length / 32) * 4;

    // handle all complete sets of 32 bytes
    for(; u.p64 < end; u.p64 += 4) {
      c += u.p64[0];
      d += u.p64[1];
      internal_spooky_hash_short_mix(a, b, c, d);
      a += u.p64[2];
      b += u.p64[3];
    }

    //Handle the case of 16+ remaining bytes.
    if(remainder >= 16) {
      c += u.p64[0];
      d += u.p64[1];
      internal_spooky_hash_short_mix(a, b, c, d);
      u.p64 += 2;
      remainder -= 16;
    }
  }

  // Handle the last 0..15 bytes, and its length
  d += ((uint64_t)length) << 56;
  switch(remainder) {
    case 15:
      d += ((uint64_t)u.p8[14]) << 48;
      // fall through
    case 14:
      d += ((uint64_t)u.p8[13]) << 40;
      // fall through
    case 13:
      d += ((uint64_t)u.p8[12]) << 32;
      // fall through
    case 12:
      d += u.p32[2];
      c += u.p64[0];
      break;
    case 11:
      d += ((uint64_t)u.p8[10]) << 16;
      // fall through
    case 10:
      d += ((uint64_t)u.p8[9]) << 8;
      // fall through
    case 9:
      d += (uint64_t)u.p8[8];
      // fall through
    case 8:
      c += u.p64[0];
      break;
    case 7:
      c += ((uint64_t)u.p8[6]) << 48;
      // fall through
    case 6:
      c += ((uint64_t)u.p8[5]) << 40;
      // fall through
    case 5:
      c += ((uint64_t)u.p8[4]) << 32;
      // fall through
    case 4:
      c += u.p32[0];
      break;
    case 3:
      c += ((uint64_t)u.p8[2]) << 16;
      // fall through
    case 2:
      c += ((uint64_t)u.p8[1]) << 8;
      // fall through
    case 1:
      c += (uint64_t)u.p8[0];
      break;
    case 0:
      c += sc_const;
      d += sc_const;
  }
  internal_spooky_hash_short_end(a, b, c, d);
  *hash1 = a;
  *hash2 = b;
}

static inline void internal_spooky_hash(const void* message, size_t length, uint64_t* hash1, uint64_t* hash2) {
  if(length < BUF_SIZE) {
    internal_spooky_hash_short(message, length, hash1, hash2);
    return;
  }

  uint64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
  uint64_t buf[NUM_VARS];
  uint64_t* end;
  union {
    const uint8_t* p8;
    uint64_t* p64;
    size_t i;
  } u;
  size_t remainder;

  h0 = h3 = h6 = h9 = *hash1;
  h1 = h4 = h7 = h10 = *hash2;
  h2 = h5 = h8 = h11 = sc_const;

  u.p8 = (const uint8_t*)message;
  end = u.p64 + (length / BLOCK_SIZE) * NUM_VARS;

  // handle all whole sc_blockSize blocks of bytes
  if(ALLOW_UNALIGNED_READS || ((u.i & 0x7) == 0)) {
    while(u.p64 < end) {
      internal_spooky_hash_mix(u.p64, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
      u.p64 += NUM_VARS;
    }
  }
  else {
    while(u.p64 < end) {
      __builtin_memcpy(buf, u.p64, BLOCK_SIZE);
      internal_spooky_hash_mix(buf, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
      u.p64 += NUM_VARS;
    }
  }

  // handle the last partial block of sc_blockSize bytes
  remainder = (length - ((const uint8_t*)end - (const uint8_t*)message));
  memcpy(buf, end, remainder);
  memset(((uint8_t*)buf) + remainder, 0, BLOCK_SIZE - remainder);
  ((uint8_t*)buf)[BLOCK_SIZE - 1] = remainder;

  // do some final mixing
  internal_spooky_hash_end(buf, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
  *hash1 = h0;
  *hash2 = h1;
}

#ifdef INT128_SUPPORTED
  uint128_t ncrypto_spooky128(const void* data, size_t length, uint128_t seed) {
    uint64_t h1 = unpack_lower_u128_u64(seed);
    uint64_t h2 = unpack_upper_u128_u64(seed);
    internal_spooky_hash(data, length, &h1, &h2);
    return pack_u64_u128(h1, h2);
  }
#endif

size_t ncrypto_spooky64(const void* data, size_t length, uint64_t seed) {
  uint64_t h1 = 0;
  uint64_t h2 = seed;
  internal_spooky_hash(data, length, &h1, &h2);
  return h1;
}