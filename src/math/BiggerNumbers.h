#ifndef SSCE_BIGGERNUMBERS
#define SSCE_BIGGERNUMBERS
/**
* @file
* @brief Utils for handling 128bit integers.
*/

#include <stdint.h>

#ifdef INT128_SUPPORTED
    #define unpack_lower_u128_u64(a) (uint64_t)((a) & UINT64_MAX)
    #define unpack_upper_u128_u64(a) (uint64_t)((a) & (((uint128_t)UINT64_MAX) << 64))
    #define pack_u64_u128(a, b) (uint128_t)((uint128_t)(a) | ((uint128_t)(b) << 64))
#endif

#endif /*SSCE_BIGGERNUMBERS*/