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
/* String from static array. */
#define StringStatic(A) ((String){A, sizeof(A)/sizeof(char) - 1})
/* String from dynamically allocated pointer. */
#define StringDynamic(p) ((String){p, strlen(p)})

/*
 * Print a string to the console,
 * using the encoding of the OS.
 */
void ssce_native_puts(const char* str);
/*
 * Concatenates multiple strings together.
 * The arguments must be String structures
 * passed in by value.
 */
String ssce_multi_concat(const size_t count, ...);

//TODO: cleanup
void memswap_sse2(void* dst, void* src, size_t len);
void memswap_generic64(void* dst, void* src, size_t len);
void memswap_generic32(void* dst, void* src, size_t len);
void memswap_avx(void* dst, void* src, size_t len);

#endif /*SSCE_STRINGS_H*/