#ifndef SSCE_STRINGS_HPP
#define SSCE_STRINGS_HPP

#include <Macros.h>
C_DECLS_START
#include "Strings.h"
C_DECLS_END

namespace ssce::strings {
/*
 * Print a string to the console,
 * using the encoding of the OS.
 */
inline void putsNative(const char* str) {
  ssce_native_puts(str);
}
//TODO: ssce_multi_concat with maybe templates?
}
#endif /*SSCE_STRINGS_HPP*/