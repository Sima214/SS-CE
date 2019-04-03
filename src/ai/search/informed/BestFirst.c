#include "BestFirst.h"

#include <Macros.h>
#include <ai/search/SearchProblem.h>
#include <memory/FAlloc.h>
#include <memory/GAlloc.h>
#include <structures/HashSet.h>
#include <structures/Interface.h>
#include <structures/SortedArray.h>

#include <stddef.h>
#include <stdint.h>

/*
 * TODO: move to a more central location.
 */
typedef struct {
  IDataType dti;
  const IDataType* old;
} RDataType;

struct BestFSState {
  // TODO: do this properly with a Heap data structure.
  // SortedArray used as frontier.
  SortedArray* frontier;
  // HashMap used as closed set.
  HashSet* closed_set;
  // Problem interface.
  const ISearchProblem* problem;
  // State data interface.
  const IDataType* interface;
  // Reverse state data interface.
  RDataType* reverse_interface;
};

static int internal_bestfs_reverse_cmp_l(const RDataType* dti, const void* a, const void* b) {
  return dti->old->cmp_l(dti->old, b, a);
}

static int internal_bestfs_reverse_cmp_le(const RDataType* dti, const void* a, const void* b) {
  return dti->old->cmp_le(dti->old, b, a);
}

BestFSState* bestfs_create(const ISearchProblem* problem, const void* initial_state) {
  BestFSState* obj = malloc(sizeof(BestFSState));
  if(obj != NULL) {
    // Create special IDataType for sorting based on the heuristic.
    RDataType* rdti = malloc(sizeof(RDataType));
    if(rdti == NULL) {
      // Could not allocate data type for frontier.
      free(obj);
      return NULL;
    }
    memcpy(rdti, problem->state_interface, sizeof(IDataType));
    rdti->dti.cmp_l = (Compare)internal_bestfs_reverse_cmp_l;
    rdti->dti.cmp_le = (Compare)internal_bestfs_reverse_cmp_le;
    rdti->old = problem->state_interface;
    // Allocate frontier/agenda.
    SortedArray* sa = sorted_array_create((IDataType*)rdti);
    if(sa == NULL) {
      // SortedArray allocation failed.
      free(rdti);
      free(obj);
      return NULL;
    }
    sorted_array_insert(sa, initial_state);
    // Allocate closed set.
    HashSet* hs = hashset_create(problem->state_interface, 0, -1.0, -1.0);
    if(hs == NULL) {
      // HashSet allocation failed.
      sorted_array_destroy(sa);
      free(obj);
      return NULL;
    }
    obj->frontier = sa;
    obj->closed_set = hs;
    obj->problem = problem;
    obj->interface = problem->state_interface;
    obj->reverse_interface = rdti;
    return obj;
  }
  else {
    return NULL;
  }
}

int bestfs_step(BestFSState* bfs, void* goal_state) {
  if(sorted_array_size(bfs->frontier) == 0) {
    // No more nodes to search.
    return -1;
  }
  // Allocate temporary variable length variable.
  void* current_state = falloc_malloc(bfs->interface->size);
  if(current_state == NULL) {
    // Allocation failed.
    return 2;
  }
  // Get first state.
  if(COLD_BRANCH(sorted_array_pop(bfs->frontier, current_state))) {
    // Pop failed.
    falloc_free(current_state);
    return 2;
  }
  // Test if current state is in the closed set.
  if(hashset_contains(bfs->closed_set, current_state)) {
    // Current state has already been searched.
    falloc_free(current_state);
    return 0;
  }
  // Test if we've reached a goal.
  if(COLD_BRANCH(bfs->problem->state_heuristic(bfs->problem, current_state) == 0)) {
    // Goal found!
    memcpy(goal_state, current_state, bfs->interface->size);
    falloc_free(current_state);
    return 1;
  }
  // Expand current state.
  TempArray children = bfs->problem->state_expand(bfs->problem, current_state);
  if(children.length != 0) {
    if(COLD_BRANCH(sorted_array_merge(bfs->frontier, children.data, children.length))) {
      // Merge failed.
      free(children.data);
      falloc_free(current_state);
      return 2;
    }
  }
  free(children.data);
  // Add current state to closed set.
  if(COLD_BRANCH(hashset_add(bfs->closed_set, current_state))) {
    // Add failed.
    falloc_free(current_state);
    return 2;
  }
  // Success. Cleanup.
  falloc_free(current_state);
  return 0;
}

void bestfs_destroy(BestFSState* bfs) {
  sorted_array_destroy(bfs->frontier);
  hashset_destroy(bfs->closed_set);
  free(bfs->reverse_interface);
  free(bfs);
}