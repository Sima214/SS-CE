#include "ai_search.h"
#include "test_utils.h"

#include <DFS.h>
#include <Macros.h>
#include <FAlloc.h>
#include <Logger.h>
#include <Macros.h>

#include <stdint.h>
#include <stdio.h>

int main(MARK_UNUSED int argc, MARK_UNUSED char* argv[]) {
  NPuzzleSearchProblem npsp;
  IDataType dti;
  // new_npuzzle(&npsp, &dti, 3);
  new_npuzzle(&npsp, &dti, 2);
  logger_logi("State is %zu bytes long.", dti.size);
  logger_logi("Max depth is %zu.", npsp.max_depth);
  void* initial_state = falloc_malloc(dti.size);
  if(initial_state == NULL) {
    puts("Could not allocate fast ram!");
    return EXIT_FAILURE;
  }
  // gen_initial_state(initial_state, 3, 6, 7, 1, 0, 3, 2, 8, 5, 4);
  gen_initial_state(initial_state, 2, 1, 2, 0, 3);
  DFSState* dfs = dfs_create((ISearchProblem*)&npsp, initial_state);
  falloc_free(initial_state);
  if(dfs == NULL) {
    puts("Could not allocate bfs state!");
    return EXIT_FAILURE;
  }
  // Start searching!
  void* solution = falloc_malloc(dti.size);
  int solution_found = 0;
  while(!solution_found) {
    EARLY_TRACE("Performing a step on dfs...");
    int r = dfs_step(dfs, solution);
    switch(r) {
      case 0:
        /* Continue */
        break;
      case 1:
        /* A solution has been found! */
        logger_logw("Found a solution!");
        print_solution(&npsp, &dti, solution);
        solution_found = 1;
        break;
      case -1:
        /* Solution NOT found */
        logger_loge("Could not find a solution!");
        return EXIT_FAILURE;
        break;
      default:
        /* Fatal error */
        logger_logf("bfs_step returned fatal error!");
        return EXIT_FAILURE;
    }
  }
  // Report results.
  // Cleanup...
  falloc_free(solution);
  dfs_destroy(dfs);
  return EXIT_SUCCESS;
}