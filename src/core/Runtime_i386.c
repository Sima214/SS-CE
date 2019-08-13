#include "Runtime.h"

#include <Macros.h>

#include <cpuid.h>
#include <stdint.h>

#define SSE_MASK MASK_CREATE(25)
#define SSE2_MASK MASK_CREATE(26)
#define SSE3_MASK MASK_CREATE(0)
#define SSSE3_MASK MASK_CREATE(9)
#define SSE41_MASK MASK_CREATE(19)
#define SSE42_MASK MASK_CREATE(20)

void internal_runtime_init_cpu(Runtime* rt) {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  if(HOT_BRANCH(__get_cpuid(1, &eax, &ebx, &ecx, &edx))) {
    rt->cpu_x86_sse = MASK_TEST(edx, SSE_MASK);
    rt->cpu_x86_sse2 = MASK_TEST(edx, SSE2_MASK);
    rt->cpu_x86_sse3 = MASK_TEST(ecx, SSE3_MASK);
    rt->cpu_x86_ssse3 = MASK_TEST(ecx, SSSE3_MASK);
    rt->cpu_x86_sse41 = MASK_TEST(ecx, SSE41_MASK);
    rt->cpu_x86_sse42 = MASK_TEST(ecx, SSE42_MASK);
  } else {
    EARLY_TRACE("Cpu does not support processor info and feature bits.");
  }
}