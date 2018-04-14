#include "Strings.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

String ssce_multi_concat(const size_t count, ...) {
  String cache[count];
  String final = { NULL, 0 };
  //Prepare vargs.
  va_list vargs;
  va_start(vargs, count);
  for(size_t i=0; i<count; i++) {
    cache[i] = va_arg(vargs, String);
    final.len += cache[i].len;
  }
  va_end(vargs);
  final.array = malloc(final.len + 1);
  if(final.array == NULL) {
    return (String){NULL, 0};
  }
  char* sp = final.array;
  for(size_t i=0; i<count; i++) {
    String cur = cache[i];
    memcpy(sp, cur.array, cur.len);
    sp += cur.len;
  }
  *(sp) = '\0';
  return final;
}