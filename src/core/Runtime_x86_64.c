#include "Runtime.h"

#include <Macros.h>

#include <stdint.h>
#include <cpuid.h>

#define SSE_MASK MASK_CREATE(25)
#define SSE2_MASK MASK_CREATE(26)
#define SSE3_MASK MASK_CREATE(0)
#define SSSE3_MASK MASK_CREATE(9)
#define SSE41_MASK MASK_CREATE(19)
#define SSE42_MASK MASK_CREATE(20)
#define F16C_MASK MASK_CREATE(29)
#define FMA_MASK MASK_CREATE(12)
#define OSXSAVE_MASK MASK_CREATE(27)
#define AVX_MASK MASK_CREATE(28)
#define AVX2_MASK MASK_CREATE(5)
#define AVX512F_MASK MASK_CREATE(16)
#define AVX512DQ_MASK MASK_CREATE(17)
#define AVX512IFMA_MASK MASK_CREATE(21)
#define AVX512PF_MASK MASK_CREATE(26)
#define AVX512ER_MASK MASK_CREATE(27)
#define AVX512CD_MASK MASK_CREATE(28)
#define AVX512BW_MASK MASK_CREATE(30)
#define AVX512VL_MASK MASK_CREATE(31)
#define AVX512VBMI_MASK MASK_CREATE(1)
#define AVX512VBMI2_MASK MASK_CREATE(6)

#define XFEATURE_ENABLED_MASK 0

static int initialized;
static Runtime features;

MARK_CONST static inline uint64_t xgetbv(unsigned int index) {
  uint32_t eax, edx;
  __asm__("xgetbv"
          : "=a"(eax), "=d"(edx)
          : "c"(index));
  return ((uint64_t)edx << 32) | eax;
}

static inline int is_avx_enabled(unsigned int ecx) {
  return MASK_TEST(ecx, OSXSAVE_MASK) && ((xgetbv(XFEATURE_ENABLED_MASK) & 0x6) == 0x6);
}

static inline int is_avx512_enabled() {
  return (xgetbv(XFEATURE_ENABLED_MASK) & 0xe6) == 0xe6;
}

Runtime* ssce_get_runtime() {
  if(!initialized) {
    features.cpu_64bit = 1;
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    if(HOT_BRANCH(__get_cpuid(1, &eax, &ebx, &ecx, &edx))) {
      features.cpu_x86_sse = MASK_TEST(edx, SSE_MASK);
      features.cpu_x86_sse2 = MASK_TEST(edx, SSE2_MASK);
      features.cpu_x86_sse3 = MASK_TEST(ecx, SSE3_MASK);
      features.cpu_x86_ssse3 = MASK_TEST(ecx, SSSE3_MASK);
      features.cpu_x86_sse41 = MASK_TEST(ecx, SSE41_MASK);
      features.cpu_x86_sse42 = MASK_TEST(ecx, SSE42_MASK);
      features.cpu_x86_f16c = MASK_TEST(ecx, F16C_MASK);
      features.cpu_x86_fma = MASK_TEST(ecx, FMA_MASK);
      if(is_avx_enabled(ecx)) {
        features.cpu_x86_avx = MASK_TEST(ecx, AVX_MASK);
        if(HOT_BRANCH(__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx))) {
          features.cpu_x86_avx2 = MASK_TEST(ebx, AVX2_MASK);
          if(is_avx512_enabled()) {
            features.cpu_x86_avx512f = MASK_TEST(ebx, AVX512F_MASK);
            features.cpu_x86_avx512dq = MASK_TEST(ebx, AVX512DQ_MASK);
            features.cpu_x86_avx512ifma = MASK_TEST(ebx, AVX512IFMA_MASK);
            features.cpu_x86_avx512pf = MASK_TEST(ebx, AVX512PF_MASK);
            features.cpu_x86_avx512er = MASK_TEST(ebx, AVX512ER_MASK);
            features.cpu_x86_avx512cd = MASK_TEST(ebx, AVX512CD_MASK);
            features.cpu_x86_avx512bw = MASK_TEST(ebx, AVX512BW_MASK);
            features.cpu_x86_avx512vl = MASK_TEST(ebx, AVX512VL_MASK);
            features.cpu_x86_avx512vbmi = MASK_TEST(ecx, AVX512VBMI_MASK);
            features.cpu_x86_avx512vbmi2 = MASK_TEST(ecx, AVX512VBMI2_MASK);
          } else {
            EARLY_TRACE("AVX512 disabled by OS!");
          }
        } else {
          EARLY_TRACE("Cpu does not support extended features.");
        }
      } else {
        EARLY_TRACE("AVX disabled by OS!")
      }
    } else {
      EARLY_TRACE("Cpu does not support processor info and feature bits.");
    }
    initialized = 1;
  }
  return &features;
}