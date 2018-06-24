#ifndef SSCE_INTERFACE_H
#define SSCE_INTERFACE_H

#include <Macros.h>
#include <stddef.h>
#include <string.h>

struct DataTypeInterface;
typedef struct DataTypeInterface DataTypeInterface;

typedef int (*Compare)(const DataTypeInterface*, void*, void*);
typedef void (*Operate)(const DataTypeInterface*, void*, void*);

/*
 * When this option is used
 * for key_type, you must provide
 * all of the callback functions.
 */
#define SSCE_INTERFACE_CUSTOM MASK_CREATE(0)
/*
 * Interprets the key as a float if set.
 * By default the key is interpreted as an integer.
 */
#define SSCE_INTERFACE_FLOAT MASK_CREATE(1)
/*
 * Interpret the key as an unsigned number.
 */
#define SSCE_INTERFACE_UNSIGNED MASK_CREATE(2)

/*
 * An interface for abstract data types.
 * Every element is of 'size' bytes.
 * However only 'key_size' bytes after
 * 'offset' bytes are used to compare
 * elements based on the value of 'key_type'.
 * NOTE: If SSCE_INTERFACE_CUSTOM is used for
 * 'key_type', then all function pointers must
 * be provided. Also the function pointers
 * receive pointers with the offset preapplied.
 */
struct DataTypeInterface {
  // Size of each element in bytes.
  size_t size;
  // Offset in bytes to apply from the start of an element.
  size_t offset;
  // Size of key for each element.
  size_t key_size;
  // The type of key. Must be a bitfield of SSCE_INTERFACE_*
  size_t key_type;
  // Returns true if data[i]==data[j].
  Compare cmp_eq;
  // Returns true if data[i]<data[j].
  Compare cmp_l;
  // Returns true if data[i]<=data[j].
  Compare cmp_le;
  // Swaps data[i] with data[j].
  Operate swap;
};

#define dti_custom(dti) MASK_TEST(dti->key_type, SSCE_INTERFACE_CUSTOM)
#define addp(p, offset) (void*)(((char*)p) + offset)

#endif /*SSCE_INTERFACE_H*/