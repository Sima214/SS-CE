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
 * Initialize a new bitfield.
 * TODO: alignment
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

#define internal_bitfield_for_each_set(byte, code_block) \
  while(byte != 0) {                                     \
    size_t bit_index = __builtin_ffs(byte) - 1;          \
    byte ^= ((__typeof__(byte))0x1) << bit_index;        \
    bit_index = base_bit_index + bit_index;              \
    (code_block);                                        \
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
 * TODO: arm. I wonder the benefit of counting the bits and predetermined loop count will have any benefit.
 * NOTE: This feels retarded.
 */
#define bitfield_for_each(obj, filter, dense, code_block)            \
  for(size_t bfe_i = 0; i < (obj)->__length / sizeof(size_t); i++) { \
    size_t value = (obj)->__data[bfe_i];                             \
    size_t base_bit_index = bfe_i * sizeof(size_t) * __CHAR_BIT__;   \
    if(filter == 0) {                                                \
      int bit_value = 0;                                             \
      if(dense && value == SIZE_MAX) {                               \
        continue;                                                    \
      }                                                              \
      value = ~value;                                                \
      uint_least8_t b0 = EXTRACT_BYTE(value, 0);                     \
      internal_bitfield_for_each_set(b0, code_block);                \
      uint_least8_t b1 = EXTRACT_BYTE(value, 1);                     \
      internal_bitfield_for_each_set(b1, code_block);                \
      uint_least8_t b2 = EXTRACT_BYTE(value, 2);                     \
      internal_bitfield_for_each_set(b2, code_block);                \
      uint_least8_t b3 = EXTRACT_BYTE(value, 3);                     \
      internal_bitfield_for_each_set(b3, code_block);                \
      if(__SIZEOF_SIZE_T__ == 8) {                                   \
        uint_least8_t b4 = EXTRACT_BYTE(value, 4);                   \
        internal_bitfield_for_each_set(b4, code_block);              \
        uint_least8_t b5 = EXTRACT_BYTE(value, 5);                   \
        internal_bitfield_for_each_set(b5, code_block);              \
        uint_least8_t b6 = EXTRACT_BYTE(value, 6);                   \
        internal_bitfield_for_each_set(b6, code_block);              \
        uint_least8_t b7 = EXTRACT_BYTE(value, 7);                   \
        internal_bitfield_for_each_set(b7, code_block);              \
      }                                                              \
    } else if(filter == 1) {                                         \
      int bit_value = 1;                                             \
      if(!dense && value == 0) {                                     \
        continue;                                                    \
      }                                                              \
      uint_least8_t b0 = EXTRACT_BYTE(value, 0);                     \
      internal_bitfield_for_each_set(b0, code_block);                \
      uint_least8_t b1 = EXTRACT_BYTE(value, 1);                     \
      internal_bitfield_for_each_set(b1, code_block);                \
      uint_least8_t b2 = EXTRACT_BYTE(value, 2);                     \
      internal_bitfield_for_each_set(b2, code_block);                \
      uint_least8_t b3 = EXTRACT_BYTE(value, 3);                     \
      internal_bitfield_for_each_set(b3, code_block);                \
      if(__SIZEOF_SIZE_T__ == 8) {                                   \
        uint_least8_t b4 = EXTRACT_BYTE(value, 4);                   \
        internal_bitfield_for_each_set(b4, code_block);              \
        uint_least8_t b5 = EXTRACT_BYTE(value, 5);                   \
        internal_bitfield_for_each_set(b5, code_block);              \
        uint_least8_t b6 = EXTRACT_BYTE(value, 6);                   \
        internal_bitfield_for_each_set(b6, code_block);              \
        uint_least8_t b7 = EXTRACT_BYTE(value, 7);                   \
        internal_bitfield_for_each_set(b7, code_block);              \
      }                                                              \
    } else {                                                         \
    }                                                                \
  }

/**
 * Deinit an object.
 */
static inline FORCE_INLINE void bitfield_deinit(MARK_UNUSED Bitfield* obj) {
  // NOP
}

#endif /*SSCE_BITFIELD*/