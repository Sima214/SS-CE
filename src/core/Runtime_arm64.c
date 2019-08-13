#include "Runtime.h"

#include <Macros.h>

#include <asm/hwcap.h>
#include <sys/auxv.h>

void internal_runtime_init_cpu(Runtime* rt) {
  // unsigned long hwcap = getauxval(AT_HWCAP);
  // unsigned long hwcap2 = getauxval(AT_HWCAP2);
}