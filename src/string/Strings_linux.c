#include "Strings.h"

#include <stdio.h>

/*
 * Let's assume linux is running in UTF-8.
 * There is a environment variable to check this,
 * but I will not bother with that.
 */
void ssce_native_puts(const char* str) {
  puts(str);
}