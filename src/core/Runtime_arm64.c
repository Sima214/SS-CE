#include "Runtime.h"

#include <Macros.h>

#include <sys/auxv.h>
#include <asm/hwcap.h>

static int initialized;
static Runtime features;

Runtime* ssce_get_runtime() {
  if(!initialized) {
    // unsigned long hwcap = getauxval(AT_HWCAP);
    // unsigned long hwcap2 = getauxval(AT_HWCAP2);
    features.cpu_64bit = 1;
    initialized = 1;
  }
  return &features;
}