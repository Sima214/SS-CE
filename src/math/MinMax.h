#ifndef SSCE_MINMAX_H
#define SSCE_MINMAX_H
/**
 * @file
 * @brief Methods about finding the maximum or minimum from a set of integers.
 */

/**
 * Returns the maximum of the two inputs.
 */
#define math_max(a, b)      \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
  })

/**
 * Returns the minimum of the two inputs.
 */
#define math_min(a, b)      \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
  })

#endif /*SSCE_MINMAX_H*/