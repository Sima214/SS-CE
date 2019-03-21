#ifndef SSCE_INTERFACE_H
#define SSCE_INTERFACE_H
/**
 * @file
 * @brief Interface for data structures elements.
 */

#include <Macros.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct IDataType;
typedef struct IDataType IDataType;

/**
 * Receives pointers with the offset preapplied.
 * @return A boolean style value.
 */
typedef int (*Compare)(const IDataType*, void*, void*);

/**
 * Receives pointers with no offset preapplied.
 */
typedef void (*Operate)(const IDataType*, void*, void*);

/**
 * An interface for abstract data types.
 * Every element is of \ref size bytes.
 * Only \ref key_size bytes after
 * \ref offset bytes are used
 * as the the 'key' of your element.
 * IMPORTANT: \ref Compare functions
 * receive directly pointer to the keys,
 * unlike \ref Operate functions which receive
 * pointer to the start of the element.
 */
struct IDataType {
  /** Size of each element in bytes. */
  size_t size;
  /** Offset in bytes to apply from the start of an element. */
  size_t offset;
  /** Size of key for each element. */
  size_t key_size;
  /** A pointer to a function which returns true if data[i] == data[j]. */
  Compare cmp_eq;
  /** A pointer to a function which returns true if data[i] < data[j]. */
  Compare cmp_l;
  /** A pointer to a function which returns true if data[i] <= data[j]. */
  Compare cmp_le;
  /** A pointer to a function which swaps data[i] with data[j]. */
  Operate swap;
};

#define add_offset(p, offset) (void*)(((char*)p) + offset)
#define get_address(p, index, size) add_offset(p, (index) * (size))
#define dti_element(dti, p, index) get_address(p, index, dti->size)
#define dti_item(dti, p, index) add_offset(dti_element(dti, p, index), dti->offset)
#define dti_previous(dti, item) add_offset(item, (-dti->size))

#endif /*SSCE_INTERFACE_H*/