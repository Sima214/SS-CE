#include <stdio.h>

#include <structures/Interface.h>
#include <structures/Sort.h>

int main(int argc, char* argv[]) {
  DataTypeInterface i = {NULL};
  heapsort(NULL, 0, i);
}