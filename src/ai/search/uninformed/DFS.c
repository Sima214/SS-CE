#include "DFS.h"

#include <Macros.h>
#include <ai/search/SearchProblem.h>
#include <memory/FAlloc.h>
#include <memory/GAlloc.h>
#include <structures/Dequeue.h>
#include <structures/HashSet.h>
#include <structures/Interface.h>

#include <stddef.h>
#include <stdint.h>

struct DFSState {
  // Queue used as frontier.
  Dequeue* frontier;
  // HashMap used as closed set.
  HashSet* closed_set;
  // Problem interface.
  const ISearchProblem* problem;
  // State data interface.
  const IDataType* interface;
};

DFSState* dfs_create(const ISearchProblem* problem, const void* initial_state) {
  DFSState* obj = malloc(sizeof(DFSState));
  if(obj != NULL) {
    // Allocate frontier/agenda.
    Dequeue* dq = dequeue_create(problem->state_interface);
    if(dq != NULL) {
      if(dequeue_push_front(dq, initial_state)) {
        // Insert failed.
        free(obj);
        return NULL;
      }
    }
    else {
      // Dequeue allocation failed.
      free(obj);
      return NULL;
    }
    // Allocate closed set.
    HashSet* hs = hashset_create(problem->state_interface, 0, -1.0, -1.0);
    if(hs == NULL) {
      // HashSet allocation failed.
      dequeue_destroy(dq);
      free(obj);
      return NULL;
    }
    obj->frontier = dq;
    obj->closed_set = hs;
    obj->problem = problem;
    obj->interface = problem->state_interface;
    return obj;
  }
  else {
    return NULL;
  }
}

int dfs_step(DFSState* dfs, void* goal_state) {
  if(dequeue_size(dfs->frontier) == 0) {
    // No more nodes to search.
    return -1;
  }
  // Allocate temporary variable length variable.
  void* current_state = falloc_malloc(dfs->interface->size);
  if(current_state == NULL) {
    // Allocation failed.
    return 2;
  }
  // Get first state.
  if(COLD_BRANCH(dequeue_pop_front(dfs->frontier, current_state))) {
    // Pop failed.
    falloc_free(current_state);
    return 2;
  }
  // Test if current state is in the closed set.
  if(hashset_contains(dfs->closed_set, current_state)) {
    // Current state has already been searched.
    falloc_free(current_state);
    return 0;
  }
  // Test if we've reached a goal.
  if(COLD_BRANCH(dfs->problem->state_heuristic(dfs->problem, current_state) == 0)) {
    // Goal found!
    memcpy(goal_state, current_state, dfs->interface->size);
    falloc_free(current_state);
    return 1;
  }
  // Expand current state and add generated children to the frontier.
  TempArray children = dfs->problem->state_expand(dfs->problem, current_state);
  for(size_t i = 0; i < children.length; i++) {
    // TODO: add more batch operations.
    void* c = dti_element(dfs->interface, children.data, i);
    if(hashset_contains(dfs->closed_set, c)) {
      continue;
    }
    if(COLD_BRANCH(dequeue_push_front(dfs->frontier, c))) {
      // Insertion failed.
      free(children.data);
      falloc_free(current_state);
      return 2;
    }
  }
  free(children.data);
  // Add current state to closed set.
  if(COLD_BRANCH(hashset_add(dfs->closed_set, current_state))) {
    // Add failed.
    falloc_free(current_state);
    return 2;
  }
  // Success. Cleanup.
  falloc_free(current_state);
  return 0;
}

void dfs_destroy(DFSState* dfs) {
  dequeue_destroy(dfs->frontier);
  hashset_destroy(dfs->closed_set);
  free(dfs);
}