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

#ifndef NDEBUG
  #include <stdlib.h>
  #define BOUNDS_CHECK(i, l)                                                \
    if(i >= l) {                                                            \
      EARLY_TRACEF("Index out of bounds(%zu>=%zu) at %s.", i, l, __func__); \
      abort();                                                              \
    }
#else
  #define BOUNDS_CHECK(i, l)
#endif

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
 * Given a size in bytes, returns how many bytes should actually be allocated.
 */
static inline FORCE_INLINE size_t bitfield_size(size_t bytes) {
  size_t rem = bytes % sizeof(size_t);
  if(rem == 0) {
    return bytes;
  } else {
    return bytes + (sizeof(size_t) - rem);
  }
}

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
  return !!(length % sizeof(size_t));
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
  BOUNDS_CHECK(word, obj->__length / sizeof(size_t));
  obj->__data[word] |= ((size_t)0x1) << bit;
}

/**
 * Clear a bit.
 */
static inline FORCE_INLINE void bitfield_clear(Bitfield* obj, size_t index) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  BOUNDS_CHECK(word, obj->__length / sizeof(size_t));
  obj->__data[word] &= ~(((size_t)0x1) << bit);
}

/**
 * Toggle a bit.
 */
static inline FORCE_INLINE void bitfield_toggle(Bitfield* obj, size_t index) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  BOUNDS_CHECK(word, obj->__length / sizeof(size_t));
  obj->__data[word] ^= ((size_t)0x1) << bit;
}

/**
 * Set a bit to a value.
 */
static inline FORCE_INLINE void bitfield_assign(Bitfield* obj, size_t index, int value) {
  size_t word = index / SIZE_T_BITS;
  unsigned int bit = index % SIZE_T_BITS;
  BOUNDS_CHECK(word, obj->__length / sizeof(size_t));
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
  BOUNDS_CHECK(word, obj->__length / sizeof(size_t));
  size_t mask = ((size_t)0x1) << bit;
  size_t value = obj->__data[word] & mask;
  return !!value;
}

#define internal_bitfield_for_each_set(byte, byte_index, code_block) \
  while(byte != 0) {                                                 \
    MARK_UNUSED size_t bit_index = __builtin_ffs(byte) - 1;          \
    byte ^= ((__typeof__(byte))0x1) << bit_index;                    \
    bit_index += base_bit_index + byte_index * CHAR_BIT;             \
    (code_block);                                                    \
  }

#define internal_bitfield_for_each(byte, byte_index, code_block) \
  for(size_t ibte_i = 0; ibte_i < CHAR_BIT; ibte_i++) {          \
    MARK_UNUSED int bit_value = byte & 0x1;                      \
    byte = byte >> 1;                                            \
    MARK_UNUSED size_t bit_index = ibte_i;                       \
    bit_index += base_bit_index + byte_index * CHAR_BIT;         \
    (code_block);                                                \
  }

/**
 * Executes code_block for each element in the bitfield with optinal.
 * 
 * @param obj Pointer to a \ref Bitfield.
 * @param filter 0 calls \p code_block only for cleared bits, 1 only for set bits.
 * Any other value disables filtering.
 * @param dense if filtering is enabled extra optimizations are made.
 * A non-zero value means that bits are set more often than cleared
 * @param code_block literally a block of code.
 * Has the same locals variables as the line where \ref bitfield_for_each is called(parameters included).
 * Also it has access to a variable `size_t bit_index` which contains the index of the found bit
 * and `int bit_value` the value of the found bit.
 * TODO: arm optimizations, counting the bits optimization for intel branch predictor.
 */
#define bitfield_for_each(obj, filter, dense, code_block)                    \
  for(size_t bfe_i = 0; bfe_i < (obj)->__length / sizeof(size_t); bfe_i++) { \
    size_t bfe_value = (obj)->__data[bfe_i];                                 \
    size_t base_bit_index = bfe_i * sizeof(size_t) * CHAR_BIT;               \
    if(filter == 0) {                                                        \
      MARK_UNUSED int bit_value = 0;                                         \
      if(dense && bfe_value == SIZE_MAX) {                                   \
        continue;                                                            \
      }                                                                      \
      bfe_value = ~bfe_value;                                                \
      uint_least8_t bfe_b0 = EXTRACT_BYTE(bfe_value, 0);                     \
      internal_bitfield_for_each_set(bfe_b0, 0, code_block);                 \
      uint_least8_t bfe_b1 = EXTRACT_BYTE(bfe_value, 1);                     \
      internal_bitfield_for_each_set(bfe_b1, 1, code_block);                 \
      uint_least8_t bfe_b2 = EXTRACT_BYTE(bfe_value, 2);                     \
      internal_bitfield_for_each_set(bfe_b2, 2, code_block);                 \
      uint_least8_t bfe_b3 = EXTRACT_BYTE(bfe_value, 3);                     \
      internal_bitfield_for_each_set(bfe_b3, 3, code_block);                 \
      if(__SIZEOF_SIZE_T__ == 8) {                                           \
        uint_least8_t bfe_b4 = EXTRACT_BYTE(bfe_value, 4);                   \
        internal_bitfield_for_each_set(bfe_b4, 4, code_block);               \
        uint_least8_t bfe_b5 = EXTRACT_BYTE(bfe_value, 5);                   \
        internal_bitfield_for_each_set(bfe_b5, 5, code_block);               \
        uint_least8_t bfe_b6 = EXTRACT_BYTE(bfe_value, 6);                   \
        internal_bitfield_for_each_set(bfe_b6, 6, code_block);               \
        uint_least8_t bfe_b7 = EXTRACT_BYTE(bfe_value, 7);                   \
        internal_bitfield_for_each_set(bfe_b7, 7, code_block);               \
      }                                                                      \
    } else if(filter == 1) {                                                 \
      MARK_UNUSED int bit_value = 1;                                         \
      if(!dense && bfe_value == 0) {                                         \
        continue;                                                            \
      }                                                                      \
      uint_least8_t bfe_b0 = EXTRACT_BYTE(bfe_value, 0);                     \
      internal_bitfield_for_each_set(bfe_b0, 0, code_block);                 \
      uint_least8_t bfe_b1 = EXTRACT_BYTE(bfe_value, 1);                     \
      internal_bitfield_for_each_set(bfe_b1, 1, code_block);                 \
      uint_least8_t bfe_b2 = EXTRACT_BYTE(bfe_value, 2);                     \
      internal_bitfield_for_each_set(bfe_b2, 2, code_block);                 \
      uint_least8_t bfe_b3 = EXTRACT_BYTE(bfe_value, 3);                     \
      internal_bitfield_for_each_set(bfe_b3, 3, code_block);                 \
      if(__SIZEOF_SIZE_T__ == 8) {                                           \
        uint_least8_t bfe_b4 = EXTRACT_BYTE(bfe_value, 4);                   \
        internal_bitfield_for_each_set(bfe_b4, 4, code_block);               \
        uint_least8_t bfe_b5 = EXTRACT_BYTE(bfe_value, 5);                   \
        internal_bitfield_for_each_set(bfe_b5, 5, code_block);               \
        uint_least8_t bfe_b6 = EXTRACT_BYTE(bfe_value, 6);                   \
        internal_bitfield_for_each_set(bfe_b6, 6, code_block);               \
        uint_least8_t bfe_b7 = EXTRACT_BYTE(bfe_value, 7);                   \
        internal_bitfield_for_each_set(bfe_b7, 7, code_block);               \
      }                                                                      \
    } else {                                                                 \
      uint_least8_t bfe_b0 = EXTRACT_BYTE(bfe_value, 0);                     \
      internal_bitfield_for_each(bfe_b0, 0, code_block);                     \
      uint_least8_t bfe_b1 = EXTRACT_BYTE(bfe_value, 1);                     \
      internal_bitfield_for_each(bfe_b1, 1, code_block);                     \
      uint_least8_t bfe_b2 = EXTRACT_BYTE(bfe_value, 2);                     \
      internal_bitfield_for_each(bfe_b2, 2, code_block);                     \
      uint_least8_t bfe_b3 = EXTRACT_BYTE(bfe_value, 3);                     \
      internal_bitfield_for_each(bfe_b3, 3, code_block);                     \
      if(__SIZEOF_SIZE_T__ == 8) {                                           \
        uint_least8_t bfe_b4 = EXTRACT_BYTE(bfe_value, 4);                   \
        internal_bitfield_for_each(bfe_b4, 4, code_block);                   \
        uint_least8_t bfe_b5 = EXTRACT_BYTE(bfe_value, 5);                   \
        internal_bitfield_for_each(bfe_b5, 5, code_block);                   \
        uint_least8_t bfe_b6 = EXTRACT_BYTE(bfe_value, 6);                   \
        internal_bitfield_for_each(bfe_b6, 6, code_block);                   \
        uint_least8_t bfe_b7 = EXTRACT_BYTE(bfe_value, 7);                   \
        internal_bitfield_for_each(bfe_b7, 7, code_block);                   \
      }                                                                      \
    }                                                                        \
  }

/**
 * Deinit an object.
 */
static inline FORCE_INLINE void bitfield_deinit(MARK_UNUSED Bitfield* obj) {
  // NOP
}

#endif /*SSCE_BITFIELD*/