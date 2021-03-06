#include "Runtime.h"

#include <Config.h>

static Runtime instance;

void ssce_get_version(int* major, int* minor, int* release) {
  *major = SSCE_VERSION_MAJOR;
  *minor = SSCE_VERSION_MINOR;
  *release = SSCE_VERSION_PATCH;
}

Runtime* ssce_get_runtime() {
  return &instance;
}

void internal_runtime_init() {
  instance.cpu_64bit = __SIZEOF_POINTER__ > 4;
  internal_runtime_init_cpu(&instance);
  internal_runtime_init_os(&instance);
}