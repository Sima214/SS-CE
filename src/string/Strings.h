#ifndef SSCE_STRINGS_H
#define SSCE_STRINGS_H
/**
 * @file
 * @brief Extra string methods.
 */

#include <stddef.h>

/**
 * Size aware strings in C.
 */
typedef struct {
  /** Data pointer. May not be null terminated. */
  char* array;
  /** String length in bytes. */
  size_t len;
} String;

/** String from static null terminated array. */
#define StringStatic(A) ((String){A, sizeof(A) - 1})
/** String from dynamically allocated null terminated pointer. */
#define StringDynamic(p) ((String){p, strlen(p)})

/**
 * Print a string to stdout,
 * using the encoding of the OS.
 * 
 * @param str The null terminated string to output.
 */
void native_puts(const char* str);
/**
 * Concatenates multiple strings together.
 * 
 * @param count String structures count.
 * @param ... String structures passed in by value.
 * @return String structure which is the result of
 * concatenation of all the input String structures.
 * Result is null terminated malloced pointer(the caller must free).
 */
String multi_concat(const size_t count, ...);
/**
 * Swaps len bytes from the two input memory location.
 * The locations pointed to by both parameters,
 * not overlap (else the results are undefined).
 * 
 * @param dst The first memory location.
 * @param src The second memory location.
 * @param len How many bytes to swap.
 */
void memswap(void* dst, void* src, size_t len);

#endif /*SSCE_STRINGS_H*/