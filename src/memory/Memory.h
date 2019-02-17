#ifndef SSCE_SWAP_H
#define SSCE_SWAP_H
/**
 * @file
 * @brief A collection of functions about memory manipulation.
 */

#include <Macros.h>

#include <stddef.h>

/**
 * Swaps len bytes from the two input memory location.
 * The locations pointed to by both parameters must
 * not overlap (else the results are undefined).
 * There are no special alignment requirements,
 * however miss-alignment may impact performance.
 * 
 * @param dst The first memory location.
 * @param src The second memory location.
 * @param len How many bytes to swap.
 */
EXPORT_API void memswap(void* dst, void* src, size_t len);

#endif /*SSCE_SWAP_H*/