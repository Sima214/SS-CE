#ifndef SSCE_MATH_EXTRA
#define SSCE_MATH_EXTRA
/**
* @file
* @brief Extra math operations on scalar integers and floats.
*/

#include <Macros.h>

#include <math.h>
#include <stdint.h>

EXPORT_API int8_t sqrt_i8(int8_t a);
EXPORT_API int16_t sqrt_i16(int16_t a);
EXPORT_API int32_t sqrt_i32(int32_t a);
EXPORT_API int64_t sqrt_i64(int64_t a);
EXPORT_API uint8_t sqrt_u8(uint8_t a);
EXPORT_API uint16_t sqrt_u16(uint16_t a);
EXPORT_API uint32_t sqrt_u32(uint32_t a);
EXPORT_API uint64_t sqrt_u64(uint64_t a);
EXPORT_API float sqrt_f32(float a);
EXPORT_API double sqrt_f64(double a);

/**
 * Automatically choose a sqrt implementation based on the type of the argument.
 */
#define ssce_sqrt(a) _Generic( \
  (a),                         \
  int8_t: sqrt_i8,             \
  int16_t: sqrt_i16,           \
  int32_t: sqrt_i32,           \
  int64_t: sqrt_i64,           \
  uint8_t: sqrt_u8,            \
  uint16_t: sqrt_u16,          \
  uint32_t: sqrt_u32,          \
  uint64_t: sqrt_u64,          \
  float: sqrt_f32,             \
  double: sqrt_f64,            \
  default: sqrt                \
  )(a)

#endif /*SSCE_MATH_EXTRA*/