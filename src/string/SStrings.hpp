#ifndef SSCE_STRINGS_HPP
#define SSCE_STRINGS_HPP
/**
 * @file
 * @brief Extra string methods.
 */

#include <Macros.h>
C_DECLS_START
#include <SStrings.h>
C_DECLS_END

namespace ssce {

/**
 * Print a string to stdout,
 * using the encoding of the OS.
 * 
 * @param str The null terminated string to output.
 */
inline void nativePuts(const char* str) {
  native_puts(str);
}

/**
 * Not reccomended for C++ code.
 * Prefer the STL string class.
 */
template<typename... S>
String multiConcat(const S... args) {
  multi_concat(sizeof...(S), args...);
}

} // namespace ssce
#endif /*SSCE_STRINGS_HPP*/