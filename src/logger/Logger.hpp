#ifndef SSCE_LOGGER_HPP
#define SSCE_LOGGER_HPP

#include <Macros.h>
C_DECLS_START
#include <Logger.h>
C_DECLS_END

namespace ssce {
template<class... T>
void logv(T... args) {
  ssce_logv(args...);
}
template<class... T>
void logd(T... args) {
  ssce_logd(args...);
}
template<class... T>
void logi(T... args) {
  ssce_logi(args...);
}
template<class... T>
void logw(T... args) {
  ssce_logw(args...);
}
template<class... T>
void loge(T... args) {
  ssce_loge(args...);
}
template<class... T>
void logf(T... args) {
  ssce_logf(args...);
}
inline void setLogLevel(LogLevel l) {
  ssce_set_log_level(l);
}
}  // namespace ssce
#endif /*SSCE_LOGGER_HPP*/