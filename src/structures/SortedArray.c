#include "SortedArray.h"

#include <Macros.h>
#include <memory/GAlloc.h>
#include <structures/Interface.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

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

/**
 * A data structure containing a size and a boolean.
 */
typedef struct {
  size_t size;
  int boolean;
} SizeBool;


/*
 * Internal functions to provide better inlining of commonly used functions.
 */

static inline size_t internal_sorted_array_size(SortedArray* sa) {
  return sa->length;
}

static inline void* internal_sorted_array_pointer(SortedArray* sa) {
  uintptr_t base = (uintptr_t)sa->allocated;
  uintptr_t offset = sa->start_offset * sa->interface->size;
  void* fixed_address = (void*)(base + offset);
  return fixed_address;
}

static inline SizeBool internal_sorted_array_find(const IDataType* idt, void* base_address, size_t n, void* key_address) {
  if(n == 0) {
    //Avoid unsigned underflow.
    return (SizeBool){0, 0};
  }
  size_t start = 0;
  size_t end = n - 1;
  while(start <= end) {
    size_t middle = start + (end - start) / 2;
    void* middle_address = dti_item(idt, base_address, middle);
    if(idt->cmp_eq(idt, middle_address, key_address)) {
      // Hit
      return (SizeBool){middle, 1};
    }
    else if(idt->cmp_l(idt, middle_address, key_address)) {
      //Left
      end = middle - 1;
    }
    else {
      //Right
      start = middle + 1;
    }
  }
  return (SizeBool){start, 0};
}

static inline int internal_sorted_array_insert(SortedArray* sa, void* start_address, size_t length, void* value, size_t index) {
  // Make space, and then copy value at index.
  if(length == 0) {
    // No memory is allocated.
    sa->allocated = malloc(sa->interface->size);
    if(sa->allocated == NULL) {
      // Out of memory.
      EARLY_TRACE("internal_sorted_array_insert could not allocate new memory block!");
      return 0;
    }
    else {
      EARLY_TRACE("internal_sorted_array_insert allocated new memory block!");
    }
    memcpy(sa->allocated, value, sa->interface->size);
    // Everything was good. Finalize changes.
    sa->length++;
    return 1;
  }
  // Check if we have enough space in the front.
  // Also check if it's beneficial to move the first half of the array.
  else if((sa->start_offset > 0) && (index <= (length / 2))) {
    // Use already allocated space at the start of the memory block.
    EARLY_TRACE("internal_sorted_array_insert at front!");
    void* new_start_address = dti_previous(sa->interface, start_address);
    memmove(new_start_address, start_address, sa->interface->size * index);
    memcpy(dti_element(sa->interface, new_start_address, index), value, sa->interface->size);
    sa->start_offset--;
    // Everything was good. Finalize changes.
    sa->length++;
    return 1;
  }
  // We have to move the end of the array to make space.
  else {
    EARLY_TRACE("internal_sorted_array_insert at back!");
    // Make space by reallocating memory block.
    void* new_allocated = realloc(sa->allocated, sa->interface->size * (sa->length + sa->start_offset + 1));
    if(new_allocated == NULL) {
      // Out of memory.
      EARLY_TRACE("internal_sorted_array_insert could not reallocate memory block!");
      return 0;
    }
    else if(new_allocated != sa->allocated) {
      // Realloc moved allocation.
      EARLY_TRACE("internal_sorted_array_insert relocated memory block!");
      sa->allocated = new_allocated;
      start_address = internal_sorted_array_pointer(sa);
    }
    else {
      // Realloc just gave us access to more of the previously allocated memory.
      EARLY_TRACE("internal_sorted_array_insert reallocated memory block in place!");
    }
    // Make space by moving index to end.
    void* dest = dti_element(sa->interface, start_address, index);
    memmove(dti_next(sa->interface, dest), dest, sa->interface->size * index);
    memcpy(dest, value, sa->interface->size);
    // Everything was good. Finalize changes.
    sa->length++;
    return 1;
  }
}

/*
 * Api/Exported functions.
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
  return internal_sorted_array_size(sa);
}

void* sorted_array_pointer(SortedArray* sa) {
  return internal_sorted_array_pointer(sa);
}

size_t sorted_array_find(SortedArray* sa, void* value) {
  // Get current state.
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  // Add offset to value.
  void* value_key = add_offset(value, sa->interface->offset);
  // Perform binary search.
  SizeBool index_bool = internal_sorted_array_find(sa->interface, start_address, length, value_key);
  // Test mask if value was actually found.
  if(!index_bool.boolean) {
    return INVALID_SIZE_T;
  }
  else {
    return index_bool.size;
  }
}

int sorted_array_get(SortedArray* sa, void* result, size_t index) {
  // Get base address.
  void* start_address = internal_sorted_array_pointer(sa);
  // Perform bound checking.
  size_t length = internal_sorted_array_size(sa);
  if(COLD_BRANCH(index >= length)) {
    return 1;
  }
  void* index_address = dti_element(sa->interface, start_address, index);
  memcpy(result, index_address, sa->interface->size);
  return 0;
}

size_t sorted_array_insert(SortedArray* sa, void* value) {
  // Get current state.
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  // Add offset to value.
  void* value_key = add_offset(value, sa->interface->offset);
  // First find the address the new item is going to go in.
  size_t index = internal_sorted_array_find(sa->interface, start_address, length, value_key).size;
  if(internal_sorted_array_insert(sa, start_address, length, value, index)) {
    return index;
  }
  else {
    return INVALID_SIZE_T;
  }
}