#ifndef SSCE_LOGGER_H
#define SSCE_LOGGER_H
/**
 * @file
 * @brief Logging subsystem.
 * Inspired from the log4j configuration I use.
 */

#include <Macros.h>

#include <errno.h>
#include <stddef.h>

/**
 * Logging levels.
 * From less important(verbose) to very important(fatal).
 * The enums ALL and OFF should not be used for log messages.
 */
typedef enum {
  /**
   * A filter which catches all messages.
   * This should never be used as the level parameter of \ref logger_log()
   */
  LOGGER_ALL,
  /**
   * For not important messages.
   */
  LOGGER_VERBOSE,
  /**
   * For not important messages to the end user.
   */
  LOGGER_DEBUG,
  /**
   * Generic messages.
   */
  LOGGER_INFO,
  /**
   * Show a warning.
   */
  LOGGER_WARN,
  /**
   * Show an error message.
   * Normal program execution might still be possible.
   */
  LOGGER_ERROR,
  /**
   * Show a fatal error message.
   * Normal program execution is not possible.
   */
  LOGGER_FATAL,
  /**
   * A filter which disables all messages.
   * This should never be used as the level parameter of \ref logger_log()
   */
  LOGGER_OFF
} LogLevel;

/**
 * Mask used in the options parameter of \ref logger_log()
 * Posts message and then aborts execution.
 */
#define LOGGER_ABORT MASK_CREATE(0)

/**
 * Set log level.
 * Any messages sent to the logger which are less important than
 * the set level will be ignored.
 */
EXPORT_API void logger_set_level(LogLevel);

/**
 * Gets the current log level.
 */
EXPORT_API LogLevel logger_get_level();

/**
 * Low level logger.
 * All other log functions are rooted through this function.
 */
EXPORT_API void logger_log(const LogLevel level, const int options, const char* fmt, ...) MARK_PRINTF(3, 4);

/**
 * Logs a verbose message.
 * Takes the same parameters as printf.
 */
#define logger_logv(...) logger_log(LOGGER_VERBOSE, 0, __VA_ARGS__);

/**
 * Logs a debug message.
 * Takes the same parameters as printf.
 */
#define logger_logd(...) logger_log(LOGGER_DEBUG, 0, __VA_ARGS__);

/**
 * Logs a generic/info message.
 * Takes the same parameters as printf.
 */
#define logger_logi(...) logger_log(LOGGER_INFO, 0, __VA_ARGS__);

/**
 * Logs a warning message.
 * Takes the same parameters as printf.
 */
#define logger_logw(...) logger_log(LOGGER_WARN, 0, __VA_ARGS__);

/**
 * Logs an error message.
 * Takes the same parameters as printf.
 */
#define logger_loge(...) logger_log(LOGGER_ERROR, 0, __VA_ARGS__);

/**
 * Logs a fatal message and aborts execution.
 * Takes the same parameters as printf.
 */
#define logger_logf(...) logger_log(LOGGER_FATAL, LOGGER_ABORT, __VA_ARGS__);

/**
 * Checks if a pointer is null.
 * If it is, it makes the whole program crash.
 * But before that it logs a helpful message.
 */
#define logger_assert_null(p)                                                                      \
  if(COLD_BRANCH(p == NULL)) {                                                                     \
    logger_logf("Null pointer assertion at %s in %s:%d", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
  }

/**
 * Checks if \p exp evaluates to 0 (false).
 * If it does, then it makes the whole program crash.
 * Also it logs the type of error that occurred if \p exp sets errno.
 * If errno was not set, then a generic error message is printed instead.
 */
#define logger_assert_errno(exp)                                             \
  errno = 0;                                                                 \
  int r = exp;                                                               \
  if(COLD_BRANCH(r == 0)) {                                                  \
    logger_logf("%s at %s in %s:%d",                                         \
      COLD_BRANCH(errno == 0) ? "Unknown error" : strerror(errno), \
      __PRETTY_FUNCTION__, __FILE__, __LINE__);                    \
    }
  
/**
 * Used internally for initializing this module.
 */
void internal_logger_init();

/**
 * Used internally for cleaning up this module.
 */
void internal_logger_exit();

#endif /*SSCE_LOGGER_H*/