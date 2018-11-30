#include "Heap.h"

#include <stddef.h>
#include <stdint.h>

#include <Interface.h>
#include <Strings.h>

#define heap_parent_index(i) ((i - 1) / 2)
#define heap_left_index(i) (i * 2 + 1)
#define heap_right_index(i) (i * 2 + 2)
/*
 * The address of the left child of element e is going to be:
 * = array + child_index*size
 * = array + 2*size*[(e-array)/size] + size
 */
#define heap_child_cache(a, dti) (uintptr_t)(dti->offset + dti->size - (uintptr_t)a)
#define heap_left(e, c) (void*)(2 * (uintptr_t)e + c)
#define heap_right(left, dti) add_offset(left, dti->size)

void sift_down_heap(void* array, void* start, void* end, const IDataType* interface) {
  void* root = add_offset(start, interface->offset);
  end = add_offset(end, interface->offset);
  uintptr_t child_cache = heap_child_cache(array, interface);
  if(dti_custom(interface)) {
    while(heap_left(root, child_cache) <= end) {
      void* child = heap_left(root, child_cache);
      void* swap = root;
      // Test left child.
      if(interface->cmp_l(interface, swap, child)) {
        swap = child;
      }
      // Test right child.
      child = heap_right(child, interface);
      if(child <= end && interface->cmp_l(interface, swap, child)) {
        swap = child;
      }
      if(swap == root) {
        // Exit.
        return;
      } else {
        // Remove offset.
        void* root_noof = add_offset(root, -interface->offset);
        void* swap_noof = add_offset(swap, -interface->offset);
        interface->swap(interface, root_noof, swap_noof);
        root = swap;
      }
    }
  } else {
    while(heap_left(root, child_cache) <= end) {
      void* child = heap_left(root, child_cache);
      void* swap = root;
      // Test left child.
      if(dti_cmp_l(interface, swap, child)) {
        swap = child;
      }
      // Test right child.
      child = heap_right(child, interface);
      if(child <= end && dti_cmp_l(interface, swap, child)) {
        swap = child;
      }
      if(swap == root) {
        // Exit.
        return;
      } else {
        // Remove offset.
        void* root_noof = add_offset(root, -interface->offset);
        void* swap_noof = add_offset(swap, -interface->offset);
        memswap(root_noof, swap_noof, interface->size);
        root = swap;
      }
    }
  }
}

void create_heap(void* array, size_t size, const IDataType* interface) {
  size_t start = heap_parent_index(size - 1) + 1;
  do {
    start -= 1;
    sift_down_heap(array, dti_element(interface, array, start), dti_element(interface, array, size - 1), interface);
  } while(start != 0);
}