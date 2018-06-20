#ifndef SSCE_INTERFACE_H
#define SSCE_INTERFACE_H

#include <stddef.h>
#include <string.h>

typedef int (*Compare)(const void* data, size_t i, size_t j);
typedef void (*Operate)(void* data, size_t i, size_t j);

// TODO: docs
#define SSCE_INTERFACE_CUSTOM
#define SSCE_INTERFACE_FLOAT
#define SSCE_INTERFACE_UNSIGNED

/*
 * An interface for abstract data types.
 */
typedef struct {
  // Size of each element in bytes.
  size_t size;
  // Offset to apply on each element start address.
  size_t offset;
  // Size of key of each element.
  size_t key_size;
  // The type of key. It's a bitfield of SSCE_INTERFACE_* values.
  size_t key_type;
  // Returns true if data[i]==data[j].
  Compare cmp_eq;
  // Returns true if data[i]<data[j].
  Compare cmp_l;
  // Returns true if data[i]<=data[j].
  Compare cmp_le;
  // Swaps data[i] with data[j].
  Operate swap;
} DataTypeInterface;

#define dti_cmp_eq(arr, inter, i, j) (inter->cmp_eq?inter->cmp_eq(arr, i, j):(memcmp(arr+i, arr+j, inter->size)==0))

#define dti_cmp_l(arr, inter, i, j) (inter->cmp_l?inter->cmp_l(arr, i, j):(memcmp(arr+i, arr+j, inter->size)<0))

#define dti_cmp_le(arr, inter, i, j) (inter->cmp_le?inter->cmp_le(arr, i, j):(memcmp(arr+i, arr+j, inter->size)<=0))

#define dti_op_swap(arr, inter, i, j) {\
                                        if(inter->swap) {\
                                          inter->swap(arr, i, j);\
                                        }\
                                        else {\
                                          char tmp[inter->size];\
                                          memcpy(tmp, arr+i, inter->size);\
                                          memcpy(arr+i, arr+j, inter->size);\
                                          memcpy(arr+j, tmp, inter->size);\
                                        }\
                                      }\

#endif /*SSCE_INTERFACE_H*/