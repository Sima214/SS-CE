#ifndef SSCE_BITFIELD
#define SSCE_BITFIELD
/**
* @file
* @brief Header library to operate a block of memory as a bitfield.
*/

#include <Macros.h>

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SIZE_T_BITS (sizeof(size_t) * CHAR_BIT)

/**
 * Data type used for all api functions.
 * The programmer should allocate stack space for it,
 * and treat it as an opaque data type after that.
 * 
 * Note: size_t is the most `natural` data type, which is both fast and big.
 */
typedef struct {
  size_t* __data;
  size_t __length;
} Bitfield;

/**
 * Initialize a new bitfield.
 * 
 * @param obj must refer to valid storage.
 * @param data backing array.
 * @param length size of data in bytes.
 * @returns non zero on error.
 */
static inline FORCE_INLINE int bitfield_init(Bitfield* obj, void* data, size_t length) {
  obj->__data = data;
  obj->__length = length;
  return 0;
}

/**
 * Update backing storage.
 * Usefull if you want to realloc.
 * 
 * Same interface as \ref bitfield_init.
 */
static inline FORCE_INLINE int bitfield_update(Bitfield* obj, void* data, size_t length) {
  obj->__data = data;
  obj->__length = length;
  return 0;
}

/**
 * Clears all bits.
 */
static inline FORCE_INLINE void bitfield_clear_all(Bitfield* obj) {
  memset(obj->__data, 0x00, obj->__length);
}

/**
 * Set all bits.
 */
static inline FORCE_INLINE void bitfield_set_all(Bitfield* obj) {
  memset(obj->__data, 0xff, obj->__length);
}

/**
 * Set a bit.
 */
static inline FORCE_INLINE void bitfield_set(Bitfield* obj, size_t index) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  obj->__data[word] |= ((size_t)0x1) << bit;
}

/**
 * Clear a bit.
 */
static inline FORCE_INLINE void bitfield_clear(Bitfield* obj, size_t index) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  obj->__data[word] &= ~(((size_t)0x1) << bit);
}

/**
 * Toggle a bit.
 */
static inline FORCE_INLINE void bitfield_toggle(Bitfield* obj, size_t index) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  obj->__data[word] ^= ((size_t)0x1) << bit;
}

/**
 * Set a bit to a value.
 */
static inline FORCE_INLINE void bitfield_assign(Bitfield* obj, size_t index, int value) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  size_t mask = ((size_t)0x1) << bit;
  if(value) {
    obj->__data[word] |= mask;
  } else {
    obj->__data[word] &= ~mask;
  }
}

/**
 * Retrieve a bit.
 */
static inline FORCE_INLINE int bitfield_get(Bitfield* obj, size_t index) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  size_t mask = ((size_t)0x1) << bit;
  size_t value = obj->__data[word] & mask;
  return !!value;
}

/**
 * Deinit an object.
 */
static inline FORCE_INLINE void bitfield_deinit(MARK_UNUSED Bitfield* obj) {
  // NOP
}

#endif /*SSCE_BITFIELD*/