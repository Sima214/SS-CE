#include "Strings.h"

#include <emmintrin.h>
#include <immintrin.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <math/MinMax.h>

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

#define alignment(p, a) ((size_t)p)%a
#define WORD_S sizeof(size_t)
#define XMM_S sizeof(__m128i)
#define YMM_S sizeof(__m256i)

void __attribute__((__target__("sse2"))) memswap_sse2(void* const dst, void* const src, size_t len) {
  __m128i_u* xp1 = dst;
  __m128i_u* xp2 = src;
  for(/*NOOP*/; len<=XMM_S; len-=XMM_S) {
    __asm__(
      "movdqu xmm0,XMMWORD PTR [%0]\n\t"
      "movdqu xmm1,XMMWORD PTR [%1]\n\t"
      "movdqu XMMWORD PTR [%0],xmm1\n\t"
      "movdqu XMMWORD PTR [%1],xmm0;"
      : /* No output */
      : "r" (xp1), "r" (xp2) );
    xp1++;
    xp2++;
  }
}