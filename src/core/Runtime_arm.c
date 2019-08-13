#include "Runtime.h"

#include <Macros.h>

#include <asm/hwcap.h>
#include <sys/auxv.h>

void internal_runtime_init_cpu(Runtime* rt) {
  unsigned long hwcap = getauxval(AT_HWCAP);
  // unsigned long hwcap2 = getauxval(AT_HWCAP2);
  rt->cpu_64bit = 0;
  rt->cpu_arm_thumb = MASK_TEST(hwcap, HWCAP_THUMB);
  rt->cpu_arm_vfpv3 = MASK_TEST(hwcap, HWCAP_VFPv3) || MASK_TEST(hwcap, HWCAP_VFPv3D16);
  rt->cpu_arm_vfpv4 = MASK_TEST(hwcap, HWCAP_VFPv4);
  rt->cpu_arm_vfpd32 = MASK_TEST(hwcap, HWCAP_VFPD32);
  rt->cpu_arm_neon = MASK_TEST(hwcap, HWCAP_NEON);
  rt->cpu_arm_half_float = MASK_TEST(hwcap, HWCAP_HALF);
}