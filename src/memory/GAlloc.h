#ifndef SSCE_GALLOC_H
#define SSCE_GALLOC_H
/**
 * @file
 * @brief This file provides a generic malloc. This is usually provided jemalloc, but this may change in the future without notice.
 */

// First include std malloc, so we can later override them and not cause any problems.
#include <stdlib.h>

// Now include jemalloc, which is compiled with je_ prefix.
#ifndef VALGRIND
    #define JEMALLOC_NO_RENAME
    #define JEMALLOC_MANGLE
    #include <jemalloc.h>
#endif

#endif /*SSCE_GALLOC_H*/