#include "Runtime.h"

#include <Macros.h>

#include <sys/auxv.h>
#include <asm/hwcap.h>

static int initialized;
static Runtime features;

Runtime* ssce_get_runtime() {
  if(!initialized) {
    unsigned long hwcap = getauxval(AT_HWCAP);
    // unsigned long hwcap2 = getauxval(AT_HWCAP2);
    features.cpu_64bit = 0;
    features.cpu_arm_thumb = MASK_TEST(hwcap, HWCAP_THUMB);
    features.cpu_arm_vfpv3 = MASK_TEST(hwcap, HWCAP_VFPv3) || MASK_TEST(hwcap, HWCAP_VFPv3D16);
    features.cpu_arm_vfpv4 = MASK_TEST(hwcap, HWCAP_VFPv4);
    features.cpu_arm_vfpd32 = MASK_TEST(hwcap, HWCAP_VFPD32);
    features.cpu_arm_neon = MASK_TEST(hwcap, HWCAP_NEON);
    features.cpu_arm_half_float = MASK_TEST(hwcap, HWCAP_HALF);
    initialized = 1;
  }
  return &features;
}