#include "SStrings.h"

#include <memory/FAlloc.h>
#include <memory/GAlloc.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

String string_concat(const size_t count, ...) {
  // Allocate temporary and return memory.
  String* cache = falloc_malloc(count * sizeof(String));
  String final = {NULL, 0};
  // Prepare vargs.
  va_list vargs;
  va_start(vargs, count);
  // Get parameters.
  for(size_t i = 0; i < count; i++) {
    cache[i] = va_arg(vargs, String);
    final.len += cache[i].len;
  }
  va_end(vargs);
  // Allocate destination string.
  final.array = malloc(final.len + 1);
  if(final.array == NULL) {
    return (String){NULL, 0};
  }
  // Copy old strings to new string.
  char* sp = final.array;
  for(size_t i = 0; i < count; i++) {
    String cur = cache[i];
    memcpy(sp, cur.array, cur.len);
    sp += cur.len;
  }
  *(sp) = '\0';
  falloc_free(cache);
  return final;
}