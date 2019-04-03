#include "ai_search.h"
#include "test_utils.h"

#include <BestFirst.h>
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
  logger_logi("State is %u bytes long.", (uint32_t)dti.size);
  logger_logi("Max depth is %u.", (uint32_t)npsp.max_depth);
  void* initial_state = falloc_malloc(dti.size);
  if(initial_state == NULL) {
    puts("Could not allocate fast ram!");
    return EXIT_FAILURE;
  }
  // gen_initial_state(initial_state, 3, 6, 7, 1, 0, 3, 2, 8, 5, 4);
  gen_initial_state(initial_state, 2, 1, 2, 0, 3);
  BestFSState* bfs = bestfs_create((ISearchProblem*)&npsp, initial_state);
  falloc_free(initial_state);
  if(bfs == NULL) {
    puts("Could not allocate bfs state!");
    return EXIT_FAILURE;
  }
  // Start searching!
  void* solution = falloc_malloc(dti.size);
  int solution_found = 0;
  while(!solution_found) {
    EARLY_TRACE("Performing a step on bfs...");
    int r = bestfs_step(bfs, solution);
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
        logger_logf("bestfs_step returned fatal error!");
        return EXIT_FAILURE;
    }
  }
  // Report results.
  // Cleanup...
  falloc_free(solution);
  bestfs_destroy(bfs);
  return EXIT_SUCCESS;
}