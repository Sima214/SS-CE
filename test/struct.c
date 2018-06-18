#include <stdio.h>

#include <structures/Interface.h>
#include <structures/Sort.h>
#include <stddef.h>

static int char_cmp_eq(const char* data, size_t i, size_t j) {
  return data[i] == data[j];
}

static int char_cmp_l(const char* data, size_t i, size_t j) {
  return data[i] < data[j];
}

static int char_cmp_le(const char* data, size_t i, size_t j) {
  return data[i] <= data[j];
}

static void char_swap(char* data, size_t i, size_t j) {
  char tmp = data[i];
  data[i] = data[j];
  data[j] = tmp;
}

int main(int argc, char* argv[]) {
  char test[] = {0, 31, 4, 11, 24, 12, 1, 8, 2, 1, 2, 4, 5, 1, 21, 32, 13, 12, 21, 51, 53, 52, 41, 221, 3, 34, 38, 23, 45, 63, 0, 0};
  const DataTypeInterface i = {sizeof(char), (Compare) char_cmp_eq, (Compare) char_cmp_l, (Compare) char_cmp_le, (Operate) char_swap};
  const DataTypeInterface i0 = {sizeof(char), NULL, NULL, NULL, NULL};
  heapsort(test, sizeof(test)/sizeof(char), &i0);
}