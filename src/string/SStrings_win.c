#include "SStrings.h"

#include <Macros.h>

#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

/*
 * Convert string to UTF-16 and then write it to the console.
 */
void native_puts(const char* str) {
  // First pass: find required buffer size.
  int count = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
  if(COLD_BRANCH(count == 0)) {
    #ifndef NDEBUG
      uint32_t error = GetLastError();
      switch(error) {
        case ERROR_INSUFFICIENT_BUFFER:
          puts("ERROR_INSUFFICIENT_BUFFER");
          break;
        case ERROR_INVALID_FLAGS:
          puts("ERROR_INVALID_FLAGS");
          break;
        case ERROR_INVALID_PARAMETER:
          puts("ERROR_INVALID_PARAMETER");
          break;
        case ERROR_NO_UNICODE_TRANSLATION:
          puts("ERROR_NO_UNICODE_TRANSLATION");
          break;
        default:
          puts("UNKNOWN_ERROR");
          break;
      }
    #endif
    abort();
  }
  // TODO: allocate buffer
  wchar_t str_u16[count];
  int actual_count = MultiByteToWideChar(CP_UTF8, 0, str, -1, str_u16, count);
  if(COLD_BRANCH(count == 0)) {
    #ifndef NDEBUG
      uint32_t error = GetLastError();
      switch(error) {
        case ERROR_INSUFFICIENT_BUFFER:
          puts("ERROR_INSUFFICIENT_BUFFER");
          break;
        case ERROR_INVALID_FLAGS:
          puts("ERROR_INVALID_FLAGS");
          break;
        case ERROR_INVALID_PARAMETER:
          puts("ERROR_INVALID_PARAMETER");
          break;
        case ERROR_NO_UNICODE_TRANSLATION:
          puts("ERROR_NO_UNICODE_TRANSLATION");
          break;
        default:
          puts("UNKNOWN_ERROR");
          break;
      }
    #endif
    abort();
  }
  else if(COLD_BRANCH(count != actual_count)) {
    #ifndef NDEBUG
      puts("MultiByteToWideChar Fail")
    #endif
    abort();
  }
  // Get console and TODO: verify
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written = 0;
  int success = WriteConsole(console, str_u16, actual_count - 1, &written, NULL);
  if(COLD_BRANCH(success == 0)) {
    #ifndef NDEBUG
      puts("WriteConsole Fail")
    #endif
    abort();
  }
}