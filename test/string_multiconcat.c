#include <stdlib.h>
#include <string.h>

#include <Macros.h>
#include <Strings.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  const char TEST_STRING[] = "I am a test string!!!";
  String part0 = {(char*)TEST_STRING, 5};
  String part1 = {(char*)TEST_STRING+5, 9};
  String part2 = {(char*)TEST_STRING+5+9, sizeof(TEST_STRING)-1-5-9};
  String result = multi_concat(3, part0, part1, part2);
  if(!strequal(TEST_STRING, result.array)) {
    return EXIT_FAILURE;
  }
  native_puts(result.array);
  free(result.array);
  return EXIT_SUCCESS;
}