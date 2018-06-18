#include "Sort.h"

#include <structures/Interface.h>
#include <structures/Heap.h>
#include <stddef.h>

void heapsort(void* array, size_t size, const DataTypeInterface* interface) {
  createHeap(array, size, interface);
  size_t end = size - 1;
  //TODO: avoid undeflow conditions.
  while(end > 0) {
    dti_op_swap(array, interface, end, 0);
    end--;
    siftDownHeap(array, interface, 0, end);
  }
}