#include "Heap.h"

#include <stddef.h>

#define parentHeap(i) ((i-1)/2)
#define leftChildHeap(i) (i*2+1)
#define rightChildHeap(i) (i*2+2)

void siftDownHeap(void* array, const DataTypeInterface* interface, size_t start, size_t end) {
  size_t root = start;
  while(leftChildHeap(root) <= end) {
    size_t child = leftChildHeap(root);
    size_t swap = root;
    //Test left child.
    if(dti_cmp_l(array, interface, swap, child)) {
      swap = child;
    }
    //Test right child.
    child += 1;
    if(child<=end && dti_cmp_l(array, interface, swap, child)) {
      swap = child;
    }
    if(swap == root) {
      return;
    }
    else {
      dti_op_swap(array, interface, root, swap);
      root = swap;
    }
  }
}

void createHeap(void* array, size_t size, const DataTypeInterface* interface) {
  size_t start = parentHeap(size - 1) + 1;
  do {
    start -= 1;
    siftDownHeap(array, interface, start, size - 1);
  } while(start != 0);
}