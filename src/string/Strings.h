#ifndef SSCE_STRINGS_H
#define SSCE_STRINGS_H

#include <stddef.h>

/*
 * Size aware strings in C.
 */
typedef struct {
    char* array;
    size_t len;
} String;

/*
 * Print a string to the console,
 * using the character encoding
 * of the native system.
 */
void ssce_native_puts(const char* str);
/*
 * Concat multiple strings together.
 */
String concat(const int count, ...);
#endif /*SSCE_STRINGS_H*/