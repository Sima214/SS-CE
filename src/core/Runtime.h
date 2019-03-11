#ifndef SSCE_RUNTIME_H
#define SSCE_RUNTIME_H
/**
 * @file
 * @brief Retrieve information about the runtime environment.
 * TODO: other architectures.
 */

#include <Macros.h>

/**
 * Storage of collected runtime information.
 * It's mostly information about the machine
 * and the os the program is running on.
 * NOTE: changing the order of these will break binary compatibility.
 */
typedef struct {
  /**
   * 1 if the cpu uses 64bit addressing.
   */
  int cpu_64bit : 1;
  /**
   * https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
   */
  int cpu_x86_sse : 1;
  /**
   * https://en.wikipedia.org/wiki/SSE2
   */
  int cpu_x86_sse2 : 1;
  /**
   * https://en.wikipedia.org/wiki/SSE3
   */
  int cpu_x86_sse3 : 1;
  /**
   * https://en.wikipedia.org/wiki/SSSE3
   */
  int cpu_x86_ssse3 : 1;
  /**
   * https://en.wikipedia.org/wiki/SSE4#SSE4.1
   */
  int cpu_x86_sse41 : 1;
  /**
   * https://en.wikipedia.org/wiki/SSE4#SSE4.2
   */
  int cpu_x86_sse42 : 1;
  /**
   * https://en.wikipedia.org/wiki/F16C
   */
  int cpu_x86_f16c : 1;
  /**
   * https://en.wikipedia.org/wiki/FMA_instruction_set#FMA3_instruction_set
   */
  int cpu_x86_fma : 1;
  /**
   * https://en.wikipedia.org/wiki/Advanced_Vector_Extensions
   */
  int cpu_x86_avx : 1;
  /**
   * https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2
   */
  int cpu_x86_avx2 : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Foundation
   */
  int cpu_x86_avx512f : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Doubleword and Quadword Instructions
   */
  int cpu_x86_avx512dq : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Integer Fused Multiply-Add Instructions
   */
  int cpu_x86_avx512ifma : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Prefetch Instructions
   */
  int cpu_x86_avx512pf : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Exponential and Reciprocal Instructions
   */
  int cpu_x86_avx512er : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Conflict Detection Instructions
   */
  int cpu_x86_avx512cd : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Byte and Word Instructions
   */
  int cpu_x86_avx512bw : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Vector Length Extensions
   */
  int cpu_x86_avx512vl : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Vector Bit Manipulation Instructions
   */
  int cpu_x86_avx512vbmi : 1;
  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Vector Bit Manipulation Instructions 2
   */
  int cpu_x86_avx512vbmi2 : 1;
} Runtime;

/**
 * Retrieves the runtime version of SS-CE and stores it at the provided locations.
 * If using the shared version of the library,
 * then this may be different than the version used at compile time.
 */
EXPORT_API void ssce_get_version(int* major, int* minor, int* release);

/**
 * Retrieves the current \ref Runtime structure.
 */
EXPORT_API Runtime* ssce_get_runtime();

#endif /*SSCE_RUNTIME_H*/