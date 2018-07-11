#include "Strings.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

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
#define byte_swap(dst, src) {\
                    uint8_t* dstp = dst;\
                    uint8_t* srcp = src;\
                    uint8_t tmp0 = *dstp;\
                    uint8_t tmp1 = *srcp;\
                    *srcp = tmp0;\
                    *dstp = tmp1;\
                    dst = (void*) (dstp+1);\
                    src = (void*) (srcp+1);\
                  }

void memswap_generic(void* dst, void* src, size_t len) {
  // Byte swap.
  uint8_t* dstp = dst;
  uint8_t* srcp = src;
  for(/* NO-OP */; len!=0; len--) {
    uint8_t tmp0 = *dstp;
    uint8_t tmp1 = *srcp;
    *srcp = tmp0;
    *dstp = tmp1;
    dstp++;
    srcp++;
  }
}

__attribute__((__target__("sse2"))) __attribute__((optimize("no-tree-vectorize")))
void memswap_sse2(void* dst, void* src, size_t len) {
  if(len >= XMM_S) {
    asm (
      ".align 16\n"
      ".Loop_sse2:\n\t"
      "movdqu xmm0,XMMWORD PTR [%0]\n\t"
      "movdqu xmm1,XMMWORD PTR [%1]\n\t"
      "movdqu XMMWORD PTR [%1],xmm0\n\t"
      "add %1,16\n\t"
      "movdqu XMMWORD PTR [%0],xmm1\n\t"
      "add %0,16\n\t"
      "lea %2,[%2-16]\n\t"
      "cmp %2,16\n\t"
      "jge .Loop_sse2\n\t"
      : /* No output */
      : "r" (dst), "r" (src), "r" (len)
      : "xmm0", "xmm1", "cc", "memory"
    );
  }
  // Word swap.
  if((len/8) == 1) {
    len -= 8;
    uint64_t* dstp = dst;
    uint64_t* srcp = src;
    uint64_t tmp0 = *dstp;
    uint64_t tmp1 = *srcp;
    *srcp = tmp0;
    *dstp = tmp1;
    dst = (void*) (dstp+1);
    src = (void*) (srcp+1);
  }
  // Byte swap.
  uint8_t* dstp = dst;
  uint8_t* srcp = src;
  for(/* NO-OP */; len!=0; len--) {
    uint8_t tmp0 = *dstp;
    uint8_t tmp1 = *srcp;
    *srcp = tmp0;
    *dstp = tmp1;
    dstp++;
    srcp++;
  }
}
#define bulk_swap(dst, src, len, type) \

#define single_swap(dst, src, len, type) \
          if((len/sizeof(type)) == 1) {\
            len -= sizeof(type);\
            type* dstp = dst;\
            type* srcp = src;\
            type tmp0 = *dstp;\
            type tmp1 = *srcp;\
            *srcp = tmp0;\
            *dstp = tmp1;\
            dst = (void*) (dstp+1);\
            src = (void*) (srcp+1);\
          }

__attribute__((__target__("avx"))) __attribute__((optimize("no-tree-vectorize")))
void memswap_avx(void* dst, void* src, size_t len) {
  __m128i_u* dstvp = dst;
  __m128i_u* srcvp = src;
  // Bulk swap.
  while(len >= 16) {
    __m128i r0 = *dstvp;
    __m128i r1 = *srcvp;
    *(srcvp++) = r0;
    *(dstvp++) = r1;
    len -= 16;
  }
  dst = dstvp;
  src = srcvp;
  single_swap(dst, src, len, uint64_t);
  single_swap(dst, src, len, uint32_t);
  single_swap(dst, src, len, uint16_t);
  single_swap(dst, src, len, uint8_t);
}