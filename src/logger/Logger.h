#ifndef SSCE_LOGGER_H
#define SSCE_LOGGER_H

#include <Macros.h>
#include <errno.h>
#include <stddef.h>

/*
 * Logging levels.
 * From less important(verbose)
 * to very important(fatal).
 * The enums ALL and OFF should
 * not be used for log messages.
 */
typedef enum {
  ALL,
  VERBOSE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  OFF
} LogLevel;
/*
 * Masks for extra options passed to the logger.
 */
/* Posts message and then aborts execution. */
#define SSCE_LOGGER_ABORT MASK_CREATE(0)
/*
 * Set log level.
 * Any messages less important than
 * the set level will be ignored.
 */
void ssce_set_log_level(LogLevel);
/*
 * Generic logger.
 */
void ssce_log(const LogLevel level, const int options, const char* fmt, ...) MARK_PRINTF(3, 4);
/*
 * Special case loggers.
 */
#define ssce_logv(...) ssce_log(VERBOSE, 0, __VA_ARGS__);
#define ssce_logd(...) ssce_log(DEBUG, 0, __VA_ARGS__);
#define ssce_logi(...) ssce_log(INFO, 0, __VA_ARGS__);
#define ssce_logw(...) ssce_log(WARN, 0, __VA_ARGS__);
#define ssce_loge(...) ssce_log(ERROR, 0, __VA_ARGS__);
#define ssce_logf(...) ssce_log(FATAL, SSCE_LOGGER_ABORT, __VA_ARGS__);
/*
 * Checks if a pointer is null.
 * If it is, it makes the whole program crash.
 * Also it logs a helpful(?) message.
 */
#define ssce_checknull(p)                                                                        \
  if(p == NULL) {                                                                                \
    ssce_logf("Null pointer assertion at %s in %s:%d", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
  }
/*
 * Checks if a function returned a non-zero value.
 * If it has, then it makes the whole program crash.
 * Also it logs the type of error (according to errno).
 */
#define ssce_checkerrno(v)                                                               \
  if(v != 0) {                                                                           \
    logf("%s at %s in %s:%d", strerror(errno), __PRETTY_FUNCTION__, __FILE__, __LINE__); \
  }
//Internal functions, do not use outside of Lifecycle_<os>.c
void setup_log_file();
void close_log_file();
#endif /*SSCE_LOGGER_H*/