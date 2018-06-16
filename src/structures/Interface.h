#ifndef SSCE_INTERFACE_H
#define SSCE_INTERFACE_H

#include <stddef.h>

typedef int (*Compare)(void* data, size_t i, size_t j);
typedef void (*Operate)(void* data, size_t i, size_t j);

/*
 * An interface for abstract data types.
 */
typedef struct {
  // Size of data type in bytes.
  size_t size;
  // Returns true if data[i]==data[j].
  Compare cmp_eq;
  // Returns true if data[i]<data[j].
  Compare cmp_l;
  // Returns true if data[i]<=data[j].
  Compare cmp_le;
  // Swaps data[i] with data[j].
  Operate swap;
} DataTypeInterface;
#endif /*SSCE_INTERFACE_H*/