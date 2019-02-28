#include "test_utils.h"

#include <Logger.h>
#include <Macros.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  logger_logv("Trace");
  logger_logd("Debug");
  logger_logi("Information");
  logger_logw("Warning");
  logger_loge("Error");
  logger_log(LOGGER_FATAL, 0, "Fatal");
  return EXIT_SUCCESS;
}