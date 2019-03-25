#include "SortedArray.h"

#include <Macros.h>
#include <memory/FAlloc.h>
#include <memory/GAlloc.h>
#include <structures/Interface.h>
#include <structures/Sort.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#define OFFSET_THRESHOLD 128

/*
 * Implementation details:
 * if allocated is null, then length is also 0 and vise versa.
 */
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

static inline SizeBool internal_sorted_array_find(const IDataType* dti, void* base_address, size_t n, const void* key_address) {
  if(n == 0) {
    //Avoid unsigned underflow.
    return (SizeBool){0, 0};
  }
  size_t start = 0;
  size_t end = n - 1;
  while(start <= end) {
    size_t middle = start + (end - start) / 2;
    void* middle_address = dti_item(dti, base_address, middle);
    if(dti->cmp_eq(dti, middle_address, key_address)) {
      // Hit
      return (SizeBool){middle, 1};
    }
    else if(dti->cmp_l(dti, middle_address, key_address)) {
      // Right
      start = middle + 1;
    }
    else {
      // Left
      end = middle - 1;
    }
  }
  return (SizeBool){start, 0};
}

static inline SizeBool internal_sorted_array_find_stable(const IDataType* dti, void* base_address, size_t n, const void* key_address, int fwd) {
  SizeBool result = internal_sorted_array_find(dti, base_address, n, key_address);
  if(result.boolean) {
    // If we've got a hit, then find the last or first of the same element.
    size_t index = result.size;
    // Check bounds.
    if(index == (n - 1)) {
      // We are already at the end of the array.
      return result;
    }
    // Generate iterator.
    void* current = dti_item(dti, base_address, index);
    current = fwd ? dti_next(dti, current) : dti_previous(dti, current);
    index++;
    // Start search.
    while(dti->cmp_eq(dti, current, key_address)) {
      // Recheck bounds.
      if(index == (n - 1)) {
        // We are going to shoot past the array.
        result.size = index;
        return result;
      }
      current = fwd ? dti_next(dti, current) : dti_previous(dti, current);
      index++;
    }
    // We've hit something different. Go back one and return.
    result.size = index - 1;
    return result;
  }
  else {
    // Key does not exist in array.
    return result;
  }
}

static inline int internal_sorted_array_insert(SortedArray* sa, void* start_address, size_t length, const void* value, size_t index) {
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
    memmove(dti_next(sa->interface, dest), dest, sa->interface->size * (sa->length - index));
    memcpy(dest, value, sa->interface->size);
    // Everything was good. Finalize changes.
    sa->length++;
    return 1;
  }
}

static inline void internal_sorted_array_merge(void* dest, size_t dest_len, const void* src, size_t src_len, const IDataType* dti) {
  // Start from the end.
  size_t final_len = dest_len + src_len;
  void* final_elem = dti_element(dti, dest, final_len - 1);
  void* dest_elem = dti_element(dti, dest, dest_len - 1);
  void* src_elem = dti_element(dti, src, src_len - 1);
  // Loop until either input arrays run out of elements.
  while((dest_elem >= dest) && (src_elem >= src)) {
    void* dest_key = add_offset(src_elem, dti->offset);
    void* src_key = add_offset(dest_elem, dti->offset);
    if(dti->cmp_l(dti, src_key, dest_key)) {
      memcpy(final_elem, dest_elem, dti->size);
      dest_elem = dti_previous(dti, dest_elem);
    }
    else {
      memcpy(final_elem, src_elem, dti->size);
      src_elem = dti_previous(dti, src_elem);
    }
    final_elem = dti_previous(dti, final_elem);
  }
  if(dest_elem >= dest) {
    // Copy the rest of dest to final.
    // NOP (as we are merging in place).
  }
  if(src_elem >= src) {
    size_t bytes_left = ((uintptr_t)(src_elem) - (uintptr_t)(src)) + dti->size;
    memcpy(dest, src, bytes_left);
  }
}

static inline void internal_sorted_array_remove(SortedArray* sa, size_t index) {
  // Check if index is in the first half and length is large enough.
  if(index < (sa->length / 2) && sa->length > OFFSET_THRESHOLD) {
    void* base_address = internal_sorted_array_pointer(sa);
    void* new_base_address = dti_next(sa->interface, base_address);
    size_t bytes = sa->interface->size * index;
    memmove(new_base_address, base_address, bytes);
    sa->length--;
    sa->start_offset++;
  }
  else {
    // Deleted index is closer to the end.
    void* base_address = internal_sorted_array_pointer(sa);
    void* index_address = dti_element(sa->interface, base_address, index);
    void* after_address = dti_next(sa->interface, index_address);
    size_t bytes = sa->interface->size * (sa->length - index - 1);
    memmove(index_address, after_address, bytes);
    // Shrink allocated memory block.
    sa->length--;
    if(sa->length == 0) {
      free(sa->allocated);
      sa->allocated = NULL;
      sa->start_offset = 0;
    }
    else {
      void* new_allocated = realloc(sa->allocated, sa->interface->size * (sa->length + sa->start_offset));
      if(new_allocated == NULL) {
        EARLY_TRACE("internal_sorted_array_remove could not reallocate memory block!");
      }
      else if(new_allocated != sa->allocated) {
        EARLY_TRACE("internal_sorted_array_remove relocated memory block!");
        sa->allocated = new_allocated;
      }
      else {
        EARLY_TRACE("internal_sorted_array_remove reallocated memory block in place!");
      }
    }
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

size_t sorted_array_find(SortedArray* sa, const void* value) {
  // Get current state.
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  // Add offset to value.
  const void* value_key = add_offset(value, sa->interface->offset);
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

size_t sorted_array_insert(SortedArray* sa, const void* value) {
  // Get current state.
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  // Add offset to value.
  const void* value_key = add_offset(value, sa->interface->offset);
  // First find the address the new item is going to go in.
  size_t index = internal_sorted_array_find(sa->interface, start_address, length, value_key).size;
  if(internal_sorted_array_insert(sa, start_address, length, value, index)) {
    return index;
  }
  else {
    return INVALID_SIZE_T;
  }
}

size_t sorted_array_insert_stable(SortedArray* sa, const void* value) {
  // Get current state.
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  // Add offset to value.
  const void* value_key = add_offset(value, sa->interface->offset);
  // First find the address the new item is going to go in.
  size_t index = internal_sorted_array_find_stable(sa->interface, start_address, length, value_key, 1).size;
  if(internal_sorted_array_insert(sa, start_address, length, value, index)) {
    return index;
  }
  else {
    return INVALID_SIZE_T;
  }
}

int sorted_array_merge(SortedArray* sa, const void* array, size_t count) {
  // Empty dest case.
  if(sa->allocated == NULL) {
    sa->allocated = malloc(sa->interface->size * count);
    if(sa->allocated == NULL) {
      EARLY_TRACE("sorted_array_merge could not allocate new memory block!");
      return 1;
    }
    // Just copy contents and sort.
    memcpy(sa->allocated, array, sa->interface->size * count);
    sort(sa->allocated, count, sa->interface);
    sa->length = count;
    return 0;
  }
  // Create a copy of the input array.
  void* array_rw = falloc_malloc(sa->interface->size * count);
  if(array_rw == NULL) {
    EARLY_TRACE("sorted_array_merge could not allocate temp storage!");
    return 1;
  }
  memcpy(array_rw, array, sa->interface->size * count);
  // First sort input array.
  sort(array_rw, count, sa->interface);
  // Ensure destination has enough space.
  void* new_allocated = realloc(sa->allocated, sa->interface->size * (sa->start_offset + sa->length + count));
  if(new_allocated == NULL) {
    EARLY_TRACE("sorted_array_merge could not reallocate memory block!");
    return 1;
  }
  else if(new_allocated != sa->allocated) {
    EARLY_TRACE("sorted_array_merge relocated memory block!");
    sa->allocated = new_allocated;
  }
  else {
    EARLY_TRACE("sorted_array_merge reallocated memory block in place!");
  }
  // Merge sorted input array to destination.
  void* start_address = internal_sorted_array_pointer(sa);
  internal_sorted_array_merge(start_address, sa->length, array_rw, count, sa->interface);
  sa->length += count;
  // Success!
  falloc_free(array_rw);
  return 0;
}

int sorted_array_merge_sorted(SortedArray* a, SortedArray* b) {
  const IDataType* interface = a->interface;
  if(COLD_BRANCH(a == b)) {
    EARLY_TRACE("sorted_array_merge_sorted got the same array!");
    return 1;
  }
  if(b->length == 0) {
    // Nothing to do.
    return 0;
  }
  if(a->length == 0) {
    // Just copy b into a.
    size_t bytes = interface->size * b->length;
    a->allocated = malloc(bytes);
    if(a->allocated == NULL) {
      EARLY_TRACE("sorted_array_merge_sorted could not allocate new memory block!");
      return 1;
    }
    memcpy(a->allocated, b->allocated, bytes);
    return 0;
  }
  // Nothing special is going on, so just do a normal merge into a.
  // First expand a.
  void* new_a = realloc(a->allocated, interface->size * (a->start_offset + a->length + b->length));
  if(new_a == NULL) {
    EARLY_TRACE("sorted_array_merge_sorted could not reallocate memory block!");
    return 1;
  }
  else if(new_a != a->allocated) {
    EARLY_TRACE("sorted_array_merge_sorted relocated memory block!");
    a->allocated = new_a;
  }
  else {
    EARLY_TRACE("sorted_array_merge_sorted reallocated memory block in place!");
  }
  void* start_a = internal_sorted_array_pointer(a);
  void* start_b = internal_sorted_array_pointer(b);
  internal_sorted_array_merge(start_a, a->length, start_b, b->length, interface);
  a->length += b->length;
  return 0;
}

int sorted_array_erase(SortedArray* sa, size_t index) {
  // Bounds checking.
  if(index >= sa->length) {
    return 1;
  }
  // Actual remove.
  internal_sorted_array_remove(sa, index);
  return 0;
}

size_t sorted_array_delete(SortedArray* sa, const void* value) {
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  const void* key_address = add_offset(value, sa->interface->offset);
  SizeBool index_bool = internal_sorted_array_find(sa->interface, start_address, length, key_address);
  if(index_bool.boolean) {
    // Value found.
    size_t index = index_bool.size;
    internal_sorted_array_remove(sa, index);
    return index;
  }
  // Value not found.
  return INVALID_SIZE_T;
}

size_t sorted_array_delete_stable(SortedArray* sa, const void* value) {
  void* start_address = internal_sorted_array_pointer(sa);
  size_t length = internal_sorted_array_size(sa);
  const void* key_address = add_offset(value, sa->interface->offset);
  SizeBool index_bool = internal_sorted_array_find_stable(sa->interface, start_address, length, key_address, 0);
  if(index_bool.boolean) {
    // Value found.
    size_t index = index_bool.size;
    internal_sorted_array_remove(sa, index);
    return index;
  }
  // Value not found.
  return INVALID_SIZE_T;
}

void sorted_array_clear(SortedArray* sa) {
  // Most efficient way you say?
  free(sa->allocated);
  sa->allocated = NULL;
  sa->length = 0;
  sa->start_offset = 0;
}

void sorted_array_compact(SortedArray* sa) {
  if(sa->start_offset > 0) {
    // Remove offset by moving all the elements to the start.
    void* start_address = internal_sorted_array_pointer(sa);
    memmove(sa->allocated, start_address, sa->interface->size * sa->length);
    sa->start_offset = 0;
    // Free unused space.
    void* new_allocated = realloc(sa->allocated, sa->interface->size * sa->length);
    if(new_allocated == NULL) {
      EARLY_TRACE("sorted_array_compact could not reallocate memory block!");
    }
    else if(new_allocated != sa->allocated) {
      EARLY_TRACE("sorted_array_compact relocated memory block!");
      sa->allocated = new_allocated;
    }
    else {
      EARLY_TRACE("sorted_array_compact reallocated memory block in place!");
    }
  }
}

void sorted_array_destroy(SortedArray* sa) {
  free(sa->allocated);
  free(sa);
}