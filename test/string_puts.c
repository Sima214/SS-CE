#include "test_utils.h"

#include <Macros.h>
#include <Strings.h>

#include <stdlib.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  // https://www.iana.org/domains/reserved
  native_puts("test");
  native_puts("إختبار");
  native_puts("آزمایشی");
  native_puts("测试");
  native_puts("測試");
  native_puts("испытание");
  native_puts("परीक्षा");
  native_puts("δοκιμή");
  native_puts("테스트");
  native_puts("טעסט");
  native_puts("テスト");
  native_puts("பரிட்சை");
  return EXIT_SUCCESS;
}