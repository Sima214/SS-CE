#include "Dequeue.h"

#include <Macros.h>
#include <structures/Interface.h>
#include <memory/GAlloc.h>

#include <string.h>

struct Node;
typedef struct Node Node;

struct Node {
  // Previous node.
  Node* previous;
  // Next node.
  Node* next;
  // Stored data is after this point.
  // https://en.wikipedia.org/wiki/Flexible_array_member
  void* data[];
};

struct Dequeue {
  // Start of linked list.
  Node* head;
  // End of linked list.
  Node* tail;
  // Currently stored items.
  size_t length;
  // Data type definition.
  const IDataType* interface;
};

/*
 * Internal functions.
 */

static inline void internal_free_node(Node* node) {
  free(node);
}

static inline void internal_free_nodes(Node* node) {
  do {
    Node* next_node = node->previous;
    internal_free_node(node);
    node = next_node;
  } while(node != NULL);
}

static inline void internal_copy_node_data(const IDataType* dti, const Node* node, void* dest) {
  memcpy(dest, node->data, dti->size);
}

static inline Node* internal_prepare_node(const IDataType* dti, const void* data) {
  Node* new = malloc(sizeof(Node) + dti->size);
  if(new == NULL) {
    return NULL;
  }
  memcpy(new->data, data, dti->size);
  return new;
}

/*
 * Interface | Public Api.
 */

Dequeue* dequeue_create(const IDataType* dti) {
  Dequeue* obj = malloc(sizeof(Dequeue));
  obj->head = NULL;
  obj->tail = NULL;
  obj->length = 0;
  obj->interface = dti;
  return obj;
}

size_t dequeue_size(Dequeue* dq) {
  return dq->length;
}

int dequeue_push_back(Dequeue* dq, const void* data) {
  Node* new = internal_prepare_node(dq->interface, data);
  if(new == NULL) {
    return 1;
  }
  size_t len = dq->length;
  if(COLD_BRANCH(len == 0)) {
    // Nothing in the linked list.
    new->next = NULL;
    new->previous = NULL;
    dq->head = new;
    dq->tail = new;
  }
  else {
    // Normal case. Make node new tail.
    new->previous = NULL;
    new->next = dq->tail;
    dq->tail->previous = new;
    dq->tail = new;
  }
  dq->length++;
  return 0;
}

int dequeue_push_front(Dequeue* dq, const void* data) {
  Node* new = internal_prepare_node(dq->interface, data);
  if(new == NULL) {
    return 1;
  }
  size_t len = dq->length;
  if(COLD_BRANCH(len == 0)) {
    // Nothing in the linked list.
    new->next = NULL;
    new->previous = NULL;
    dq->head = new;
    dq->tail = new;
  }
  else {
    // Normal case. Make node new head.
    new->previous = dq->head;
    new->next = NULL;
    dq->head->next = new;
    dq->head = new;
  }
  dq->length++;
  return 0;
}

int dequeue_pop_back(Dequeue* dq, void* data) {
  if(HOT_BRANCH(dq->length != 0)) {
    internal_copy_node_data(dq->interface, dq->tail, data);
    return 0;
  }
  else {
    return 1;
  }
}

int dequeue_pop_front(Dequeue* dq, void* data) {
  if(HOT_BRANCH(dq->length != 0)) {
    internal_copy_node_data(dq->interface, dq->head, data);
    return 0;
  }
  else {
    return 1;
  }
}

int dequeue_peek_back(Dequeue* dq, void* data) {
  if(HOT_BRANCH(dq->length != 0)) {
    internal_copy_node_data(dq->interface, dq->tail, data);
    return 0;
  }
  else {
    return 1;
  }
}

int dequeue_peek_front(Dequeue* dq, void* data) {
  if(HOT_BRANCH(dq->length != 0)) {
    internal_copy_node_data(dq->interface, dq->head, data);
    return 0;
  }
  else {
    return 1;
  }
}

void dequeue_reset(Dequeue* dq) {
  if(dq->length != 0) {
    internal_free_nodes(dq->head);
    dq->head = NULL;
    dq->tail = NULL;
    dq->length = 0;
  }
}

void dequeue_destroy(Dequeue* dq) {
  dequeue_reset(dq);
  free(dq);
}