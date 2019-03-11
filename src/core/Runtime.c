#include "Runtime.h"

#include <Config.h>

void ssce_get_version(int* major, int* minor, int* release) {
  *major = SSCE_VERSION_MAJOR;
  *minor = SSCE_VERSION_MINOR;
  *release = SSCE_VERSION_RELEASE;
}