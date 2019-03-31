#include "HashSet.h"

#include <Config.h>
#include <Macros.h>
#include <memory/GAlloc.h>
#include <structures/Interface.h>

#ifndef NDEBUG
  #include <clock/Clock.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * A data structure containing a size and a boolean.
 */
typedef struct {
  size_t size;
  int boolean;
} SizeBool;

typedef struct {
  // Pointer to sorted bucket's data.
  void* bucket;
  // How many elements are currently stored in the bucket.
  size_t length;
} Bucket;

struct HashSet {
  // An array of buckets.
  Bucket* array;
  // How many buckets are currently allocated.
  size_t size;
  // Count of currently stored elements.
  size_t length;
  // Minimal amount of allocated buckets.
  size_t min_size;
  // When length/size<shrink, then the number of allocated buckets is decreased.
  float shrink;
  // When length/size>expand, then the number of allocated buckets is increased.
  float expand;
  // Data type definition.
  const IDataType* interface;
};

static inline SizeBool internal_bucket_find(const IDataType* dti, void* a, size_t n, const void* k) {
  if(n == 0) {
    // Avoid unsigned underflow.
    return (SizeBool){0, 0};
  }
  size_t start = 0;
  size_t end_inc = n;
  while(start < end_inc) {
    size_t middle = start + (end_inc - start) / 2;
    void* middle_address = dti_item(dti, a, middle);
    if(dti->cmp_eq(dti, middle_address, k)) {
      // Hit
      return (SizeBool){middle, 1};
    }
    else if(dti->cmp_l(dti, middle_address, k)) {
      // Right
      start = middle + 1;
    }
    else {
      // Left
      end_inc = middle;
    }
  }
  return (SizeBool){start, 0};
}

static inline int internal_bucket_insert(const IDataType* dti, Bucket* b, const void* v, size_t i) {
  // Make space, and then copy value at index.
  if(b->length == 0) {
    // No memory is allocated.
    b->bucket = malloc(dti->size);
    if(b->bucket == NULL) {
      // Out of memory.
      EARLY_TRACE("internal_bucket_insert could not allocate new bucket!");
      return 0;
    }
    memcpy(b->bucket, v, dti->size);
    // Everything was good. Finalize changes.
    b->length++;
    return 1;
  }
  // We have to move the end of the array to make space.
  else {
    if(b->length > 2) {
      EARLY_TRACEF("hashset bad distribution (%u)!", (uint32_t) (b->length + 1));
    }
    // Make space by reallocating memory block.
    void* new_bucket = realloc(b->bucket, dti->size * (b->length + 1));
    if(new_bucket == NULL) {
      // Out of memory.
      EARLY_TRACE("internal_bucket_insert could not reallocate memory block!");
      return 0;
    }
    else if(new_bucket != b->bucket) {
      // Realloc moved allocation.
      EARLY_TRACE("internal_bucket_insert relocated memory block!");
      b->bucket = new_bucket;
    }
    // Make space by moving index to end.
    void* dest = dti_element(dti, b->bucket, i);
    memmove(dti_next(dti, dest), dest, dti->size * (b->length - i));
    memcpy(dest, v, dti->size);
    // Everything is good. Finalize changes.
    b->length++;
    return 1;
  }
}

static inline void internal_bucket_remove(const IDataType* dti, Bucket* b, size_t index) {
  void* base_address = b->bucket;
  void* index_address = dti_element(dti, base_address, index);
  void* after_address = dti_next(dti, index_address);
  size_t bytes = dti->size * (b->length - index - 1);
  memmove(index_address, after_address, bytes);
  // Shrink allocated memory block.
  b->length--;
  if(b->length == 0) {
    free(b->bucket);
    b->bucket = NULL;
  }
  else {
    void* new_allocated = realloc(b->bucket, dti->size * b->length);
    if(new_allocated == NULL) {
      EARLY_TRACE("internal_bucket_remove could not reallocate memory block!");
    }
    else if(new_allocated != b->bucket) {
      EARLY_TRACE("internal_bucket_remove relocated memory block!");
      b->bucket = new_allocated;
    }
  }
}

static inline void internal_bucket_destroy(Bucket* b) {
  // Free accepts NULL pointers.
  free(b->bucket);
}

static inline void internal_buckets_destroy(Bucket b[], size_t n) {
  for(size_t i = 0; i < n; i++) {
    Bucket* c = b + i;
    internal_bucket_destroy(c);
  }
}

static inline size_t internal_hash_calc_index(const IDataType* dti, size_t max_index, const void* key) {
  return dti->hash(dti, key) % max_index;
}

static int internal_hash_resize_reloc(const IDataType* dti, Bucket* old_buckets, size_t old_size, Bucket* new_buckets, size_t new_size) {
  #ifndef NDEBUG
    PerfClock pc;
    clock_reset(&pc);
    clock_start(&pc);
  #endif
  // Iterate over all old buckets.
  for(size_t i = 0; i < old_size; i++) {
    Bucket* old_current_bucket = old_buckets + i;
    size_t old_current_bucket_length = old_current_bucket->length;
    // Each element of the old buckets gets inserted into the new buckets.
    for(size_t j = 0; j < old_current_bucket_length; j++) {
      const void* old_elem = dti_element(dti, old_current_bucket->bucket, j);
      const void* old_key = add_offset(old_elem, dti->offset);
      // Calculate destination bucket.
      size_t new_current_bucket_index = internal_hash_calc_index(dti, new_size, old_key);
      Bucket* new_current_bucket = new_buckets + new_current_bucket_index;
      // Calculate where element is going to be inserted inside the new bucket.
      size_t new_elem_index = internal_bucket_find(dti, new_current_bucket->bucket, new_current_bucket->length, old_key).size;
      if(!internal_bucket_insert(dti, new_current_bucket, old_elem, new_elem_index)) {
        EARLY_TRACEF("internal_hash_resize_reloc failure!");
        // Rollback.
        internal_buckets_destroy(new_buckets, new_size);
        return 0;
      }
    }
  }
  // Everything got moved safely, so now we can free the old buckets.
  internal_buckets_destroy(old_buckets, old_size);
  #ifndef NDEBUG
    clock_stop(&pc);
    EARLY_TRACEF("internal_hash_resize_reloc took %.4f ms!", pc.delta);
  #endif
  // Success!
  return 1;
}

static inline int internal_hash_shrink(HashSet* hs) {
  if(COLD_BRANCH(hs->shrink == 0.0)) {
    // Shrinking disabled.
    return 0;
  }
  size_t bucket_count = hs->size;
  size_t elem_count = hs->length;
  if(bucket_count <= hs->min_size) {
    // Minimum size.
    return 0;
  }
  float ratio = ((double)elem_count) / ((double)bucket_count);
  if(COLD_BRANCH(ratio < hs->expand)) {
    // Allocate new buckets.
    size_t new_bucket_count = bucket_count / STRUCTURES_HASHSET_RESIZE_FACTOR;
    Bucket* new_array = calloc(new_bucket_count, sizeof(Bucket));
    if(new_array == NULL) {
      EARLY_TRACE("internal_hash_shrink could not allocate new array!");
      return 0;
    }
    else {
      EARLY_TRACEF("internal_hash_shrink (%u -> %u)!", (uint32_t)bucket_count, (uint32_t)new_bucket_count);
    }
    // Move old elements to new buckets.
    if(internal_hash_resize_reloc(hs->interface, hs->array, hs->size, new_array, new_bucket_count)) {
      // Finalize changes.
      free(hs->array);
      hs->array = new_array;
      hs->size = new_bucket_count;
      return 1;
    }
    else {
      // Relocation failed. Rollback.
      free(new_array);
      return 0;
    }
  }
  return 0;
}

static inline int internal_hash_expand(HashSet* hs) {
  if(COLD_BRANCH(hs->expand == 0.0)) {
    // Expanding disabled.
    return 0;
  }
  size_t bucket_count = hs->size;
  size_t elem_count = hs->length;
  float ratio = ((double)elem_count) / ((double)bucket_count);
  if(COLD_BRANCH(ratio > hs->expand)) {
    // Allocate new buckets.
    size_t new_bucket_count = bucket_count * STRUCTURES_HASHSET_RESIZE_FACTOR;
    Bucket* new_array = calloc(new_bucket_count, sizeof(Bucket));
    if(new_array == NULL) {
      EARLY_TRACE("internal_hash_expand could not allocate new array!");
      return 0;
    }
    else {
      EARLY_TRACEF("internal_hash_expand (%u -> %u)!", (uint32_t)bucket_count, (uint32_t)new_bucket_count);
    }
    // Move old elements to new buckets.
    if(internal_hash_resize_reloc(hs->interface, hs->array, hs->size, new_array, new_bucket_count)) {
      // Finalize changes.
      free(hs->array);
      hs->array = new_array;
      hs->size = new_bucket_count;
      return 1;
    }
    else {
      // Relocation failed. Rollback.
      free(new_array);
      return 0;
    }
  }
  return 0;
}

HashSet* hashset_create(const IDataType* interface, size_t initial_size, float shrink_ratio, float expand_ratio) {
  HashSet* obj = malloc(sizeof(HashSet));
  if(obj != NULL) {
    if(initial_size == INVALID_SIZE_T || initial_size == 0) {
      // Use default value.
      initial_size = STRUCTURES_HASHSET_INITIAL_SIZE;
    }
    if(shrink_ratio < 0.0f) {
      // Use default value.
      shrink_ratio = STRUCTURES_HASHSET_SHRINK_RATIO;
    }
    if(expand_ratio < 0.0f) {
      // Use default value.
      expand_ratio = STRUCTURES_HASHSET_EXPAND_RATIO;
    }
    // Allocate initial bucket array.
    obj->array = calloc(initial_size, sizeof(Bucket));
    // Not enough memory.
    if(obj->array == NULL) {
      free(obj);
      return NULL;
    }
    obj->size = initial_size;
    obj->length = 0;
    obj->min_size = initial_size;
    obj->shrink = shrink_ratio;
    obj->expand = expand_ratio;
    obj->interface = interface;
  }
  return obj;
}

size_t hash_size(HashSet* hs) {
  return hs->length;
}

int hashset_contains(HashSet* hs, const void* value) {
  const void* kv = add_offset(value, hs->interface->offset);
  size_t index = internal_hash_calc_index(hs->interface, hs->size, kv);
  Bucket* buc = hs->array + index;
  int found = internal_bucket_find(hs->interface, buc->bucket, buc->length, kv).boolean;
  return found;
}

int hashset_get(HashSet* hs, void* value) {
  const void* kv = add_offset(value, hs->interface->offset);
  size_t index = internal_hash_calc_index(hs->interface, hs->size, kv);
  Bucket* buc = hs->array + index;
  SizeBool subindex_found = internal_bucket_find(hs->interface, buc->bucket, buc->length, kv);
  size_t subindex = subindex_found.size;
  int found = subindex_found.boolean;
  if(found) {
    const void* stored = dti_element(hs->interface, buc->bucket, subindex);
    memcpy(value, stored, hs->interface->size);
  }
  return !found;
}

int hashset_add(HashSet* hs, const void* value) {
  const void* kv = add_offset(value, hs->interface->offset);
  // First find out if we are replacing or adding.
  size_t index = internal_hash_calc_index(hs->interface, hs->size, kv);
  Bucket* b = hs->array + index;
  SizeBool subindex_found = internal_bucket_find(hs->interface, b->bucket, b->length, kv);
  size_t subindex = subindex_found.size;
  int found = subindex_found.boolean;
  if(found) {
    // Replacing.
    void* stored = dti_element(hs->interface, b->bucket, subindex);
    memcpy(stored, value, hs->interface->size);
    EARLY_TRACE("hashset_add replaced element!");
    return 0;
  }
  else {
    // Adding.
    if(COLD_BRANCH(internal_hash_expand(hs))) {
      // Bucket array got resized, recalculate indexes.
      size_t index = internal_hash_calc_index(hs->interface, hs->size, kv);
      Bucket* b = hs->array + index;
      size_t subindex = internal_bucket_find(hs->interface, b->bucket, b->length, kv).size;
      if(!internal_bucket_insert(hs->interface, b, value, subindex)) {
        return 1;
      }
    }
    else {
      // Bucket array didn't get resized, so we can use the indexes found above.
      if(!internal_bucket_insert(hs->interface, b, value, subindex)) {
        return 1;
      }
    }
    // Finalize changes.
    hs->length++;
    return 0;
  }
}

int hashset_remove(HashSet* hs, const void* value) {
  const void* kv = add_offset(value, hs->interface->offset);
  // First find where the element to remove is.
  size_t index = internal_hash_calc_index(hs->interface, hs->size, kv);
  Bucket* b = hs->array + index;
  SizeBool subindex_found = internal_bucket_find(hs->interface, b->bucket, b->length, kv);
  size_t subindex = subindex_found.size;
  int found = subindex_found.boolean;
  if(found) {
    // Delete.
    internal_bucket_remove(hs->interface, b, subindex);
    hs->length--;
    // Resize.
    internal_hash_shrink(hs);
    return 0;
  }
  // Not found.
  return 1;
}

int hashset_clear(HashSet* hs) {
  internal_buckets_destroy(hs->array, hs->size);
  free(hs->array);
  // Recreate buckets.
  hs->array = calloc(sizeof(Bucket), hs->min_size);
  if(hs->array == NULL) {
    return 1;
  }
  hs->size = hs->min_size;
  hs->length = 0;
  return 0;
}

void hashset_destroy(HashSet* hs) {
  internal_buckets_destroy(hs->array, hs->size);
  free(hs->array);
  free(hs);
}