#include "HashSet.h"

#include <Config.h>
#include <Macros.h>
#include <memory/GAlloc.h>

#include <stddef.h>

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
  // Data type definition.
  const IDataType* interface;
  // Minimal amount of allocated buckets.
  size_t min_size;
  // When length/size<shrink, then the number of allocated buckets is decreased.
  float shrink;
  // When length/size>expand, then the number of allocated buckets is increased.
  float expand;
};