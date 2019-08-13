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

#include <cstddef>

namespace ssce {

typedef enum {
  CPU_CACHE_L1,
  CPU_CACHE_L1I,
  CPU_CACHE_L1D,
  CPU_CACHE_L2,
  CPU_CACHE_L3
} CpuCacheIndex;

/**
 * Information about the system this code is running on.
 */
class RuntimeInfo {
 private:
  /**
   * Interface with the C portion.
   */
  Runtime* rt;

 public:
  /**
   * Loads runtime info into an object which can be used to query available information.
   */
  RuntimeInfo() {
    rt = ssce_get_runtime();
  };

  /**
   * Returns true if the cpu uses more than 32bits for addresses.
   */
  bool is64bit() {
    return rt->cpu_64bit;
  }

#ifdef arm
  /**
     * Thumb instruction set.
     */
  bool hasThumb {
    return rt->cpu_arm_thumb;
  }

  /**
     * VFPv3-D16 or above.
     */
  bool hasVfpv3 {
    return rt->cpu_arm_vfpv3;
  }

  /**
     * VFPv4-D16 or above
     */
  bool hasVfpv4 {
    return rt->cpu_arm_vfpv4;
  }

  /**
     * 32 `D` registers for VFP.
     */
  bool hasVfpd32 {
    return rt->cpu_arm_vfpd32;
  }

  /**
     * Advanced SIMD instructions.
     */
  bool hasNeon {
    return rt->cpu_arm_neon;
  }

  /**
     * Half-precision extension for NEON.
     */
  bool hasHalfFloat {
    return rt->cpu_arm_half_float;
  }

#elif defined(i386) || defined(x86_64)
  /**
     * https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
     */
  bool hasSSE() {
    return rt->cpu_x86_sse;
  };

  /**
     * https://en.wikipedia.org/wiki/SSE2
     */
  bool hasSSE2() {
    return rt->cpu_x86_sse2;
  }

  /**
     * https://en.wikipedia.org/wiki/SSE3
     */
  bool hasSSE3() {
    return rt->cpu_x86_sse3;
  }

  /**
     * https://en.wikipedia.org/wiki/SSSE3
     */
  bool hasSSSE3() {
    return rt->cpu_x86_ssse3;
  }

  /**
     * https://en.wikipedia.org/wiki/SSE4#SSE4.1
     */
  bool hasSSE41() {
    return rt->cpu_x86_sse41;
  }

  /**
     * https://en.wikipedia.org/wiki/SSE4#SSE4.2
     */
  bool hasSSE42() {
    return rt->cpu_x86_sse42;
  }
#ifdef x86_64
  /**
       * https://en.wikipedia.org/wiki/F16C
       */
  bool hasF16C() {
    return rt->cpu_x86_f16c;
  }

  /**
       * https://en.wikipedia.org/wiki/FMA_instruction_set#FMA3_instruction_set
       */
  bool hasFMA() {
    return rt->cpu_x86_fma;
  }

  /**
       * https://en.wikipedia.org/wiki/Advanced_Vector_Extensions
       */
  bool hasAVX() {
    return rt->cpu_x86_avx;
  }

  /**
       * https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2
       */
  bool hasAVX2() {
    return rt->cpu_x86_avx2;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Foundation
       */
  bool hasAVX512F() {
    return rt->cpu_x86_avx512f;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Doubleword and Quadword Instructions
       */
  bool hasAVX512DQ() {
    return rt->cpu_x86_avx512dq;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Integer Fused Multiply-Add Instructions
       */
  bool hasAVX512IFMA() {
    return rt->cpu_x86_avx512ifma;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Prefetch Instructions
       */
  bool hasAVX512PF() {
    return rt->cpu_x86_avx512pf;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Exponential and Reciprocal Instructions
       */
  bool hasAVX512ER() {
    return rt->cpu_x86_avx512er;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Conflict Detection Instructions
       */
  bool hasAVX512CD() {
    return rt->cpu_x86_avx512cd;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Byte and Word Instructions
       */
  bool hasAVX512BW() {
    return rt->cpu_x86_avx512bw;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Vector Length Extensions
       */
  bool hasAVX512VL() {
    return rt->cpu_x86_avx512vl;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Vector Bit Manipulation Instructions
       */
  bool hasAVX512VBMI() {
    return rt->cpu_x86_avx512vbmi;
  }

  /**
       * https://en.wikipedia.org/wiki/AVX-512
       * Vector Bit Manipulation Instructions 2
       */
  bool hasAVX512VBMI2() {
    return rt->cpu_x86_avx512vbmi2;
  }
#endif
#endif
  /**
   * Actual physical core count.
   */
  std::size_t getCoreCount() {
    return rt->cpu_core_count;
  }

  /**
   * Logical cores/Threads per physical core.
   * For more info research about SMT.
   */
  std::size_t getThreadCountPerCore() {
    return rt->cpu_threads_per_core;
  }

  /**
   * Total thread count.
   */
  std::size_t getThreadCount() {
    return getCoreCount() * getThreadCount();
  }

  /**
   * Smallest block of virtual memory.
   */
  std::size_t getPageSize() {
    return rt->cpu_page_size;
  }

  /**
   * Retrieves the size of the specified cache in bytes.
   */
  std::size_t getCacheSize(CpuCacheIndex i) {
    switch(i) {
      case CPU_CACHE_L1:
        return rt->cpu_cache_size_l1i + rt->cpu_cache_size_l1d;
      case CPU_CACHE_L1I:
        return rt->cpu_cache_size_l1i;
      case CPU_CACHE_L1D:
        return rt->cpu_cache_size_l1d;
      case CPU_CACHE_L2:
        return rt->cpu_cache_size_l2;
      case CPU_CACHE_L3:
        return rt->cpu_cache_size_l3;
      default:
        return 0;
    }
  }

  /**
   * Optimal pointer alignment so that data doesn't cross cache lines.
   */
  std::size_t getCacheAlignment() {
    return rt->cpu_cache_alignment;
  }
};

}  // namespace ssce

#endif /*SSCE_RUNTIME_HPP*/