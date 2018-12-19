#include "Sort.h"

#include <stddef.h>

#include <Heap.h>
#include <Interface.h>
#include <Strings.h>

void heapsort(void* array, size_t size, const IDataType* interface) {
  heap_create(array, size, interface);
  void* first = dti_element(interface, array, 0);
  void* end = dti_element(interface, array, size - 1);
  while(end > array) {
    interface->swap(interface, end, first);
    end = dti_previous(interface, end);
    heap_sift_down(array, first, end, interface);
  }
}