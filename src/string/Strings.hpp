#ifndef SSCE_STRINGS_HPP
#define SSCE_STRINGS_HPP

#include <Macros.h>
C_DECLS_START
#include <Strings.h>
C_DECLS_END

namespace ssce {
/*
 * Print a string to the console,
 * using the encoding of the OS.
 */
inline void putsNative(const char* str) {
  ssce_native_puts(str);
}
/*
 * Not reccomended for C++ code.
 * Prefer the std lib string class.
 */
template<typename... S>
String multiConcat(const S... args) {
    ssce_multi_concat(sizeof...(S), args...);
}
}
#endif /*SSCE_STRINGS_HPP*/