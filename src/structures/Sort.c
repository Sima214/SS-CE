#include "Sort.h"

#include <structures/Interface.h>
#include <structures/Heap.h>
#include <stddef.h>

void heapsort(void* array, size_t size, const IDataType* interface) {
  createHeap(array, size, interface);
  void* first = dti_item(interface, array, 0);
  void* end = dti_item(interface, array, size-1);
  while(end > array) {
    if(dti_custom(interface)) {
      interface->swap(interface, end, first);
    }
    else {
      //TODO - string extensions.
    }
    end = dti_previous(interface, end);
    siftDownHeap(array, interface, first, end);//TODO
  }
}