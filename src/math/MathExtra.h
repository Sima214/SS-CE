#ifndef SSCE_MATH_EXTRA
#define SSCE_MATH_EXTRA
/**
* @file
* @brief Extra math operations on scalar integers and floats.
*/

#include <math.h>
#include <stdint.h>

int8_t sqrt_i8(int8_t a);
int16_t sqrt_i16(int16_t a);
int32_t sqrt_i32(int32_t a);
int64_t sqrt_i64(int64_t a);
uint8_t sqrt_u8(uint8_t a);
uint16_t sqrt_u16(uint16_t a);
uint32_t sqrt_u32(uint32_t a);
uint64_t sqrt_u64(uint64_t a);

/**
 * Automatically choose a sqrt implementation based on the type of the argument.
 */
#define ssce_sqrt(a) _Generic(    \
  (a),                            \
  int8_t: sqrt_i8,                \
  int16_t: sqrt_i16,              \
  int32_t: sqrt_i32,              \
  int64_t: sqrt_i64,              \
  uint8_t: sqrt_u8,               \
  uint16_t: sqrt_u16,             \
  uint32_t: sqrt_u32,             \
  uint64_t: sqrt_u64,             \
  float: sqrtf,                   \
  double: sqrt,                   \
  default: sqrt                   \
  )(a)

#endif /*SSCE_MATH_EXTRA*/