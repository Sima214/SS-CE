#ifndef SSCE_MINMAX_HPP
#define SSCE_MINMAX_HPP
/**
 * @file
 * @brief Methods about finding the maximum or minimum from a set of integers.
 */

namespace ssce {

/**
 * Returns the maximum of the two inputs.
 */
template<class T>
T& max(T& a, T& b) {
  return a > b ? a : b;
}

/**
 * Returns the minimum of the two inputs.
 */
template<class T>
T& min(T& a, T& b) {
  return a < b ? a : b;
}

} // namespace ssce
#endif /*SSCE_MINMAX_HPP*/