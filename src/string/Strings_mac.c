#include "Strings.h"

#include <stdio.h>

/*
 * MacOS seems to be similar with linux in this case.
 */
void native_puts(const char* str) {
  puts(str);
}