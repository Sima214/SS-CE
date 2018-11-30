#ifndef SSCE_MINMAX_HPP
#define SSCE_MINMAX_HPP

namespace ssce {
template<class T>
T& max(T& a, T& b) {
  return a > b ? a : b;
}
template<class T>
T& min(T& a, T& b) {
  return a < b ? a : b;
}
}  // namespace ssce
#endif /*SSCE_MINMAX_HPP*/