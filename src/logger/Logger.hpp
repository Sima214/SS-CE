#ifndef SSCE_LOGGER_HPP
#define SSCE_LOGGER_HPP
/**
 * @file
 * @brief Logging subsystem.
 * Inspired from the log4j configuration I use.
 */

#include <Macros.h>
C_DECLS_START
#include <Logger.h>
C_DECLS_END

namespace ssce {

/**
 * Logs a verbose message.
 * Takes the same parameters as printf.
 */
template<class... T>
void logv(T... args) {
  logger_logv(args...);
}

/**
 * Logs a debug message.
 * Takes the same parameters as printf.
 */
template<class... T>
void logd(T... args) {
  logger_logd(args...);
}

/**
 * Logs a generic/info message.
 * Takes the same parameters as printf.
 */
template<class... T>
void logi(T... args) {
  logger_logi(args...);
}

/**
 * Logs a warning message.
 * Takes the same parameters as printf.
 */
template<class... T>
void logw(T... args) {
  logger_logw(args...);
}

/**
 * Logs an error message.
 * Takes the same parameters as printf.
 */
template<class... T>
void loge(T... args) {
  logger_loge(args...);
}

/**
 * Logs a fatal message and aborts execution.
 * Takes the same parameters as printf.
 */
template<class... T>
void logf(T... args) {
  logger_logf(args...);
}

/**
 * Set log level.
 * Any messages sent to the logger which are less important than
 * the set level will be ignored.
 */
inline void setLoggerLevel(LogLevel l) {
  logger_set_level(l);
}

/**
 * Gets the current log level.
 */
inline LogLevel getLoggerLevel() {
  return logger_get_level();
}

} // namespace ssce
#endif /*SSCE_LOGGER_HPP*/