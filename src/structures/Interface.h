#ifndef SSCE_INTERFACE_H
#define SSCE_INTERFACE_H
/**
 * @file
 * @brief Interface for data structures elements.
 */

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include <Macros.h>
#include <Strings.h>

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
 * Options on how to interpret data structure elements.
 */
typedef enum {
  /**
   * This option must be used
   * for \ref IDataType::key_type
   * if you want to use custom functions.
   */
  INTERFACE_TYPE_CUSTOM = MASK_CREATE(0),
  /**
   * Interprets the key as a float if set.
   * If the \ref IDataType::key_size is neither
   * the size of a float or a double, the results are undefined.
   * By default the key is interpreted as an signed integer.
   */
  INTERFACE_TYPE_FLOAT = MASK_CREATE(1),
  /**
   * Interpret the key as an unsigned number.
   * Only integers up to uintmax_t size are supported.
   */
  INTERFACE_TYPE_UNSIGNED = MASK_CREATE(2)
} INTERFACE_TYPE;

/**
 * An interface for abstract data types.
 * Every element is of \ref size bytes.
 * Only \ref key_size bytes after
 * \ref offset bytes are used to compare
 * elements based on the set bits of \ref key_type.
 */
struct IDataType {
  /** Size of each element in bytes. */
  size_t size;
  /** Offset in bytes to apply from the start of an element. */
  size_t offset;
  /** Size of key for each element. */
  size_t key_size;
  /** The type of key. Can be a combination of INTERFACE_TYPE */
  size_t key_type;
  /** Returns true if data[i]==data[j]. */
  Compare cmp_eq;
  /** Returns true if data[i]<data[j]. */
  Compare cmp_l;
  /** Returns true if data[i]<=data[j]. */
  Compare cmp_le;
  /** Swaps data[i] with data[j]. */
  Operate swap;
};

#define add_offset(p, offset) (void*)(((char*)p) + offset)
#define get_address(p, index, size) add_offset(p, (index)*(size))
#define dti_element(dti, p, index) get_address(p, index, dti->size)
#define dti_item(dti, p, index) add_offset(dti_element(dti, p, index), dti->offset)
#define dti_previous(dti, item) add_offset(item, (-dti->size))
#define dti_custom(dti) MASK_TEST(dti->key_type, INTERFACE_TYPE_CUSTOM)

#if __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__
/**
 * Transfer an odd sized unsigned int to cpu register.
 */
#define vuint2muint(dst, src, bytes)\
          {\
            const char* srcp = src;\
            for(int i=0; i<bytes; i++) {\
              uintmax_t value = *srcp;\
              dst |= (value<<(i*8));\
              srcp++;\
            }\
          }

/**
 * Transfer an odd sized integer to cpu register with sign extend.
 */
#define vint2mint(dst, src, bytes)\
          {\
            const char* srcp = src;\
            uintmax_t value;\
            for(int i=0; i<bytes; i++) {\
              value = *srcp;\
              dst |= (value<<(i*8));\
              srcp++;\
            }\
            if(value & 0x80) {\
              for(int i=bytes; i<(int)sizeof(dst); i++) {\
                dst |= (0xff<<(i*8));\
              }\
            }\
          }
#endif

MARK_CONST static inline int dti_cmp_l(const IDataType* dti, void* p1, void* p2) {
  if(MASK_TEST(dti->key_type, INTERFACE_TYPE_FLOAT)) {
    if(dti->key_size == sizeof(double)) {
      return *((double*)p1)<*((double*)p2);
    }
    else {
      #ifndef NDEBUG
      if(dti->key_size != sizeof(float)) {
        native_puts("Invalid float size!!!");
      }
      #endif
      // 'Undefined' result if the item is neither double nor float type.
      return *((float*)p1)<*((float*)p2);
    }
  }
  else if(MASK_TEST(dti->key_type, INTERFACE_TYPE_UNSIGNED)) {
    // Move the numbers to cmp1 and cmp2. Then compare those two.
    uintmax_t cmp1 = 0;
    uintmax_t cmp2 = 0;
    switch(dti->key_size) {
      case 1: {
        cmp1 = *((uint8_t*)p1);
        cmp2 = *((uint8_t*)p2);
      }
      break;
      #if UINTMAX_MAX>=UINT16_MAX
      case 2: {
        cmp1 = *((uint16_t*)p1);
        cmp2 = *((uint16_t*)p2);
      }
      break;
      #if UINTMAX_MAX>=UINT32_MAX
      case 3: {
        vuint2muint(cmp1, p1, 3);
        vuint2muint(cmp2, p2, 3);
      }
      break;
      case 4: {
        cmp1 = *((uint32_t*)p1);
        cmp2 = *((uint32_t*)p2);
      }
      break;
      #if UINTMAX_MAX>=UINT64_MAX
      case 5: {
        vuint2muint(cmp1, p1, 5);
        vuint2muint(cmp2, p2, 5);
      }
      break;
      case 6: {
        vuint2muint(cmp1, p1, 6);
        vuint2muint(cmp2, p2, 6);
      }
      break;
      case 7: {
        vuint2muint(cmp1, p1, 7);
        vuint2muint(cmp2, p2, 7);
      }
      break;
      case 8: {
        cmp1 = *((uint64_t*)p1);
        cmp2 = *((uint64_t*)p2);
      }
      #endif
      #endif
      #endif
      break;
    }
    return cmp1 < cmp2;
  }
  else {
    intmax_t cmp1 = 0;
    intmax_t cmp2 = 0;
    switch(dti->key_size) {
      case 1: {
        cmp1 = *((int8_t*)p1);
        cmp2 = *((int8_t*)p2);
      }
      break;
      #if INTMAX_MAX>=INT16_MAX
      case 2: {
        cmp1 = *((int16_t*)p1);
        cmp2 = *((int16_t*)p2);
      }
      break;
      #if INTMAX_MAX>=INT32_MAX
      case 3: {
        vint2mint(cmp1, p1, 3);
        vint2mint(cmp2, p2, 3);
      }
      break;
      case 4: {
        cmp1 = *((int32_t*)p1);
        cmp2 = *((int32_t*)p2);
      }
      break;
      #if INTMAX_MAX>=INT64_MAX
      case 5: {
        vint2mint(cmp1, p1, 5);
        vint2mint(cmp2, p2, 5);
      }
      break;
      case 6: {
        vint2mint(cmp1, p1, 6);
        vint2mint(cmp2, p2, 6);
      }
      break;
      case 7: {
        vint2mint(cmp1, p1, 7);
        vint2mint(cmp2, p2, 7);
      }
      break;
      case 8: {
        cmp1 = *((uint64_t*)p1);
        cmp2 = *((uint64_t*)p2);
      }
      break;
      #endif
      #endif
      #endif
    }
    return cmp1 < cmp2;
  }
}

#endif /*SSCE_INTERFACE_H*/