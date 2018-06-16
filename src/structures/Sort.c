#include "Sort.h"

#include <structures/Interface.h>
#include <stddef.h>

#define parentHeap(i) ((i-1)/2)
#define leftChildHeap(i) (i*2+1)
#define rightChildHeap(i) (i*2+2)

static void siftDownHeap(void* array, DataTypeInterface interface, size_t start, size_t end) {
  size_t root = start;
  while(leftChildHeap(root) <= end) {
    size_t child = leftChildHeap(root);
    size_t swap = root;
    //Test left child.
    if(interface.cmp_l(array, swap, child)) {
      swap = child;
    }
    //Test right child.
    child += 1;
    if(child<=end && interface.cmp_l(array, swap, child)) {
      swap = child;
    }
    if(swap == root) {
      return;
    }
    else {
      interface.swap(array, root, swap);
      root = swap;
    }
  }
}

static void createHeap(void* array, size_t size, DataTypeInterface interface) {
  size_t start = parentHeap(size - 1) + 1;
  do {
    start -= 1;
    siftDownHeap(array, interface, start, size - 1);
  } while(start != 0);
}

void heapsort(void* array, size_t size, DataTypeInterface interface) {
  createHeap(array, size, interface);
}