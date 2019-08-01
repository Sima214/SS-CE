#include "test_utils.hpp"

#include <Logger.hpp>
#include <Macros.h>

#include <cstdlib>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  ssce::logv("Trace");
  ssce::logd("Debug");
  ssce::logi("Information");
  ssce::logw("Warning");
  ssce::loge("Error");
  ssce::logfc("Fatal");
  return EXIT_SUCCESS;
}