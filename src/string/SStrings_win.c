#include "SStrings.h"

#include <Macros.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

static const wchar_t* NEW_LINE = L"\r\n";
static const int NEW_LINE_LEN = (sizeof(NEW_LINE) / sizeof(wchar_t)) - 1;

/**
 * Write utf-8 encoded string \ref str to a windows console window.
 */
static void native_puts_console(const char* str) {
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
      puts("MultiByteToWideChar Fail");
    #endif
    abort();
  }
  // Get console and write to it.
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written = 0;
  int success;
  success = WriteConsole(console, str_u16, actual_count - 1, &written, NULL);
  if(COLD_BRANCH(success == 0)) {
    #ifndef NDEBUG
      puts("WriteConsole Fail");
    #endif
    abort();
  }
  success = WriteConsole(console, NEW_LINE, NEW_LINE_LEN, &written, NULL);
  if(COLD_BRANCH(success == 0)) {
    #ifndef NDEBUG
      puts("WriteConsole New Line Fail");
    #endif
    abort();
  }
}

/*
 * Convert string to correct format based on what stdout is.
 */
void native_puts(const char* str) {
  // Check if stdout points to a windows console(cmd.exe).
  DWORD console_mode;
  if(GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &console_mode) == 0) {
    // Stdout is probably getting redirected!
    puts(str);
  }
  else {
    // Stdout points to a console!
    native_puts_console(str);
  }
}