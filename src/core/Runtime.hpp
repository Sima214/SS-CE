#ifndef SSCE_RUNTIME_HPP
#define SSCE_RUNTIME_HPP
/**
 * @file
 * @brief Retrieve information about the runtime environment.
 */

#include <Macros.h>
C_DECLS_START
#include <Runtime.h>
C_DECLS_END

namespace ssce {

/**
 * Information about the system this code is running on.
 */
class RuntimeInfo {
 private:
  /**
   * Interface with the C portion.
   */
  Runtime& rt;

 public:
  /**
   * Loads runtime info into an object which can be used to query available information.
   */
  RuntimeInfo() {
    rt = *ssce_get_runtime();
  };

  /**
   * Returns true if the cpu is of the 64bit kind or higher.
   */
  bool is64bit() {
    return rt.cpu_64bit;
  }

  /**
   * https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
   */
  bool hasSSE() {
    return rt.cpu_x86_sse == 1;
  };

  /**
   * https://en.wikipedia.org/wiki/SSE2
   */
  bool hasSSE2() {
    return rt.cpu_x86_sse2 == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/SSE3
   */
  bool hasSSE3() {
    return rt.cpu_x86_sse3 == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/SSSE3
   */
  bool hasSSSE3() {
    return rt.cpu_x86_ssse3 == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/SSE4#SSE4.1
   */
  bool hasSSE41() {
    return rt.cpu_x86_sse41 == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/SSE4#SSE4.2
   */
  bool hasSSE42() {
    return rt.cpu_x86_sse42 == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/F16C
   */
  bool hasF16C() {
    return rt.cpu_x86_f16c == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/FMA_instruction_set#FMA3_instruction_set
   */
  bool hasFMA() {
    return rt.cpu_x86_fma == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/Advanced_Vector_Extensions
   */
  bool hasAVX() {
    return rt.cpu_x86_avx == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2
   */
  bool hasAVX2() {
    return rt.cpu_x86_avx2 == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Foundation
   */
  bool hasAVX512F() {
    return rt.cpu_x86_avx512f == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Doubleword and Quadword Instructions
   */
  bool hasAVX512DQ() {
    return rt.cpu_x86_avx512dq == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Integer Fused Multiply-Add Instructions
   */
  bool hasAVX512IFMA() {
    return rt.cpu_x86_avx512ifma == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Prefetch Instructions
   */
  bool hasAVX512PF() {
    return rt.cpu_x86_avx512pf == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Exponential and Reciprocal Instructions
   */
  bool hasAVX512ER() {
    return rt.cpu_x86_avx512er == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Conflict Detection Instructions
   */
  bool hasAVX512CD() {
    return rt.cpu_x86_avx512cd == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Byte and Word Instructions
   */
  bool hasAVX512BW() {
    return rt.cpu_x86_avx512bw == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Vector Length Extensions
   */
  bool hasAVX512VL() {
    return rt.cpu_x86_avx512vl == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Vector Bit Manipulation Instructions
   */
  bool hasAVX512VBMI() {
    return rt.cpu_x86_avx512vbmi == 1;
  }

  /**
   * https://en.wikipedia.org/wiki/AVX-512
   * Vector Bit Manipulation Instructions 2
   */
  bool hasAVX512VBMI2() {
    return rt.cpu_x86_avx512vbmi2 == 1;
  }
};

}  // namespace ssce

#endif /*SSCE_RUNTIME_HPP*/