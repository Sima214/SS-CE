#ifndef SSCE_SEARCH_PROBLEM_H
#define SSCE_SEARCH_PROBLEM_H
/**
 * @file
 * @brief Common data structure for defining a search tree for problem solving.
 */

#include <Interface.h>

#include <stddef.h>
#include <stdint.h>

struct ISearchProblem;
typedef struct ISearchProblem ISearchProblem;

/**
 * TODO: make a proper array data structure.
 */
typedef struct {
  size_t length;
  void* data;
} TempArray;

/**
 * Receives a state and expands its children.
 * @returns An array with the children of node \p state.
 */
typedef TempArray (*ExpandState)(const ISearchProblem* isp, const void* state);

/**
 * Receives a state and performs a calculation on it.
 * @returns a signed integer value, with values closer to zero being closer to the goal.
 *   If \p state is a goal, then this must return 0.
 */
typedef int (*Heuristic)(const ISearchProblem* isp, const void* state);

/**
 * A structure which defines a graph/tree search problem.
 */
struct ISearchProblem {
  /** A \ref IDataType structure defining the type of the state. */
  const IDataType* state_interface;
  /** A pointer to a function which expands a state. */
  ExpandState state_expand;
  /** A pointer to a heuristic function */
  Heuristic state_heuristic;
};

#endif /*SSCE_SEARCH_PROBLEM_H*/