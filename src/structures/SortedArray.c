#include "SortedArray.h"

#include <Macros.h>
#include <memory/GAlloc.h>
#include <structures/Interface.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define SIZE_T_MSB_MASK MASK_CREATE((sizeof(size_t) * 8) - 1)

struct SortedArray {
  // Start of allocated memory block.
  void* allocated;
  // Amount of elements currently stored.
  size_t length;
  // Offset in elements in the start of the allocated memory block.
  size_t start_offset;
  // Interface with which this SortedArray is created.
  const IDataType* interface;
};

/*
static inline size_t internal_sorted_array_find(void* a, size_t n, void* key, IDataType* i) {
  if(n == 0) {
    //Avoid unsigned underflow.
    return 0 | SIZE_T_MSB_MASK;
  }
  size_t start = 0;
  size_t end = n - 1;
  while(start <= end) {
    int middle = start + (end - start) / 2;
    void* cur = a.at(middle);
    if(cur == key) {
      //Hit
      key = cur;
      return middle;
    }
    else if(key < cur) {
      //Left
      end = middle - 1;
    }
    else if(key > cur) {
      //Right
      start = middle + 1;
    }
  }
  return start | SIZE_T_MSB_MASK;
}
*/

SortedArray* sorted_array_create(const IDataType* interface) {
  SortedArray* ret = malloc(sizeof(SortedArray));
  ret->allocated = NULL;
  ret->length = 0;
  ret->start_offset = 0;
  ret->interface = interface;
  return ret;
}

size_t sorted_array_size(SortedArray* sa) {
  return sa->length;
}

void* sorted_array_pointer(SortedArray* sa) {
  uintptr_t base = (uintptr_t)sa->allocated;
  uintptr_t offset = sa->start_offset * sa->interface->size;
  void* fixed_address = (void*)(base + offset);
  return fixed_address;
}

/*
size_t sorted_array_find(SortedArray* sa, void* value) {
  void* start_address = sorted_array_pointer(sa);
}
*/