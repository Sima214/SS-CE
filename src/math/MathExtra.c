#include "MathExtra.h"

int8_t sqrt_i8(int8_t a) {
  double r = __builtin_sqrt(a);
  return (int8_t)(r+0.5);
}

int16_t sqrt_i16(int16_t a) {
  double r = __builtin_sqrt(a);
  return (int16_t)(r+0.5);
}

int32_t sqrt_i32(int32_t a) {
  double r = __builtin_sqrt(a);
  return (int32_t)(r+0.5);
}

int64_t sqrt_i64(int64_t a) {
  double r = __builtin_sqrt(a);
  return (int64_t)(r+0.5);
}

uint8_t sqrt_u8(uint8_t a) {
  double r = __builtin_sqrt(a);
  return (uint8_t)(r+0.5);
}

uint16_t sqrt_u16(uint16_t a) {
  double r = __builtin_sqrt(a);
  return (uint16_t)(r+0.5);
}

uint32_t sqrt_u32(uint32_t a) {
  double r = __builtin_sqrt(a);
  return (uint32_t)(r+0.5);
}

uint64_t sqrt_u64(uint64_t a) {
  double r = __builtin_sqrt(a);
  return (uint64_t)(r+0.5);
}

float sqrt_f32(float a) {
  double r = __builtin_sqrt(a);
  return (float)r;
}

double sqrt_f64(double a) {
  double r = __builtin_sqrt(a);
  return r;
}