#include "Sort.h"

#include <stddef.h>

#include <Heap.h>
#include <Interface.h>
#include <Strings.h>

void heapsort(void* array, size_t size, const IDataType* interface) {
  create_heap(array, size, interface);
  void* first = dti_element(interface, array, 0);
  void* end = dti_element(interface, array, size - 1);
  while(end > array) {
    if(dti_custom(interface)) {
      interface->swap(interface, end, first);
    }
    else {
      memswap(end, first, interface->size);
    }
    end = dti_previous(interface, end);
    sift_down_heap(array, first, end, interface);
  }
}