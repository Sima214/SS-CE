#ifndef TEST_AI_SEARCH_H
#define TEST_AI_SEARCH_H

#include <GAlloc.h>
#include <Hash.h>
#include <Interface.h>
#include <Macros.h>
#include <Memory.h>
#include <SearchProblem.h>

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CELL_UP 0x0
#define CELL_DOWN 0x1
#define CELL_LEFT 0x2
#define CELL_RIGHT 0x3
static const char* HISTORY2STR[] = {"Up", "Down", "Left", "Right"};
/**
 * Depth factor must be divisible by 4.
 */
#define DEPTH_FACTOR 8

typedef struct {
  unsigned int first : 2;
  unsigned int second : 2;
  unsigned int third : 2;
  unsigned int fourth : 2;
} MARK_PACKED HistoryCell;

/*
 * This is where the search problem is defined.
 */
typedef struct {
  // Inherit ISearchProblem.
  ISearchProblem isp;
  // N x N
  size_t n;
  // How deep can we go before we overflow?
  size_t max_depth;
  // Count of pieces.
  size_t count;
} NPuzzleSearchProblem;

/**
 * Helpers
 */
#define pos2index(n, x, y) (n * y + x)

static inline int ai_search_abs(int x) {
  return (x < 0) ? -x : x;
}

static inline void record_history(size_t* len, HistoryCell* history, unsigned int value) {
  // Old size is new last index.
  size_t cell_index = *len / 4;
  size_t cell_subindex = *len % 4;
  switch(cell_subindex) {
    case 0:
      history[cell_index].first = value;
      break;
    case 1:
      history[cell_index].second = value;
      break;
    case 2:
      history[cell_index].third = value;
      break;
    case 3:
      history[cell_index].fourth = value;
      break;
    default:
      // Unreachable state.
      abort();
      break;
  }
  (*len)++;
}

static inline int internal_npuzzle_heuristic(size_t n, const uint8_t* pieces) {
  size_t count = n * n;
  int accum = 0;
  // Check how far (manhattan) each piece is from its intended location.
  for(size_t i = 0; i < count; i++) {
    uint8_t piece = pieces[i];
    if(piece >= count) {
      abort();
    }
    int actual_x = i % n;
    int actual_y = i / n;
    int expect_x = piece % n;
    int expect_y = piece / n;
    int distance = ai_search_abs(actual_x - expect_x) + ai_search_abs(actual_y - expect_y);
    accum += distance;
  }
  return accum;
}

/**
 * 'Class' methods.
 */
static TempArray npuzzle_expand(const NPuzzleSearchProblem* isp, const void* state) {
  if(*((size_t*)state) == isp->max_depth) {
    // Reached max search depth.
    return (TempArray){0, NULL};
  }
  size_t n = isp->n;
  size_t size = isp->isp.state_interface->size;
  size_t key_size = isp->isp.state_interface->key_size;
  uint8_t empty_piece = isp->count - 1;
  uint8_t* pieces = add_offset(state, sizeof(size_t));
  // First find the 'empty piece'.
  size_t empty_index = INVALID_SIZE_T;
  for(size_t i = 0; i < isp->count; i++) {
    if(pieces[i] == empty_piece) {
      empty_index = i;
      break;
    }
  }
  if(empty_index >= isp->count) {
    puts("npuzzle_expand found memory corruption!");
    abort();
  }
  size_t empty_x = empty_index % n;
  size_t empty_y = empty_index / n;
  TempArray final = {0, NULL};
  // Up.
  if(empty_y != 0) {
    // Empty piece can move up.
    // 0. Allocate more memory.
    final.length++;
    final.data = realloc(final.data, final.length * size);
    // 1. Copy parent state.
    void* new_state = dti_element(isp->isp.state_interface, final.data, final.length - 1);
    uint8_t* new_pieces = add_offset(new_state, sizeof(size_t));
    size_t* new_history_size = (size_t*)new_state;
    HistoryCell* new_history = add_offset(new_pieces, key_size);
    memcpy(new_state, state, size);
    // 2. Modify child.
    // Index of the piece the empty piece got moved into.
    size_t replaced_index = pos2index(n, empty_x, empty_y - 1);
    new_pieces[empty_index] = new_pieces[replaced_index];
    new_pieces[replaced_index] = empty_piece;
    // 3. Increase history.
    record_history(new_history_size, new_history, CELL_UP);
  }
  // Down.
  if(empty_y != (n - 1)) {
    // Empty piece can move down.
    // 0. Allocate more memory.
    final.length++;
    final.data = realloc(final.data, final.length * size);
    // 1. Copy parent state.
    void* new_state = dti_element(isp->isp.state_interface, final.data, final.length - 1);
    uint8_t* new_pieces = add_offset(new_state, sizeof(size_t));
    size_t* new_history_size = (size_t*)new_state;
    HistoryCell* new_history = add_offset(new_pieces, key_size);
    memcpy(new_state, state, size);
    // 2. Modify child.
    // Index of the piece the empty piece got moved into.
    size_t replaced_index = pos2index(n, empty_x, empty_y + 1);
    new_pieces[empty_index] = new_pieces[replaced_index];
    new_pieces[replaced_index] = empty_piece;
    // 3. Increase history.
    record_history(new_history_size, new_history, CELL_DOWN);
  }
  // Left.
  if(empty_x != 0) {
    // Empty piece can move left.
    // 0. Allocate more memory.
    final.length++;
    final.data = realloc(final.data, final.length * size);
    // 1. Copy parent state.
    void* new_state = dti_element(isp->isp.state_interface, final.data, final.length - 1);
    uint8_t* new_pieces = add_offset(new_state, sizeof(size_t));
    size_t* new_history_size = (size_t*)new_state;
    HistoryCell* new_history = add_offset(new_pieces, key_size);
    memcpy(new_state, state, size);
    // 2. Modify child.
    // Index of the piece the empty piece got moved into.
    size_t replaced_index = pos2index(n, empty_x - 1, empty_y);
    new_pieces[empty_index] = new_pieces[replaced_index];
    new_pieces[replaced_index] = empty_piece;
    // 3. Increase history.
    record_history(new_history_size, new_history, CELL_LEFT);
  }
  // Right.
  if(empty_x != (n - 1)) {
    // Empty piece can move right.
    // 0. Allocate more memory.
    final.length++;
    final.data = realloc(final.data, final.length * size);
    // 1. Copy parent state.
    void* new_state = dti_element(isp->isp.state_interface, final.data, final.length - 1);
    uint8_t* new_pieces = add_offset(new_state, sizeof(size_t));
    size_t* new_history_size = (size_t*)new_state;
    HistoryCell* new_history = add_offset(new_pieces, key_size);
    memcpy(new_state, state, size);
    // 2. Modify child.
    // Index of the piece the empty piece got moved into.
    size_t replaced_index = pos2index(n, empty_x + 1, empty_y);
    new_pieces[empty_index] = new_pieces[replaced_index];
    new_pieces[replaced_index] = empty_piece;
    // 3. Increase history.
    record_history(new_history_size, new_history, CELL_RIGHT);
  }
  return final;
}

static int npuzzle_heuristic(const NPuzzleSearchProblem* isp, const void* state) {
  int h = internal_npuzzle_heuristic(isp->n, add_offset(state, sizeof(size_t)));
  EARLY_TRACEF("Current depth: %zu = %i", *((size_t*)state), h);
  return h;
}

static int npuzzle_state_cmp_eq(const IDataType* dti, const void* a, const void* b) {
  return !memcmp(a, b, dti->size);
}

static int npuzzle_state_cmp_l(const IDataType* dti, const void* a, const void* b) {
  size_t count = (dti->key_size / sizeof(uint8_t));
  size_t n = (size_t)(sqrt((double)count) + 0.1);
  int ah = internal_npuzzle_heuristic(n, a);
  int bh = internal_npuzzle_heuristic(n, b);
  return ah < bh;
}

static int npuzzle_state_cmp_le(const IDataType* dti, const void* a, const void* b) {
  size_t count = (dti->key_size / sizeof(uint8_t));
  size_t n = (size_t)(sqrt((double)count) + 0.1);
  int ah = internal_npuzzle_heuristic(n, a);
  int bh = internal_npuzzle_heuristic(n, b);
  return ah <= bh;
}

static void npuzzle_state_swap(const IDataType* dti, void* a, void* b) {
  memswap(a, b, dti->size);
}

static size_t npuzzle_state_hash(const IDataType* dti, const void* a) {
  size_t hash = ncrypto_hash(a, dti->key_size);
  return hash;
}

/**
 * Takes a pointer to a valid NPuzzle structures and initializes them.
 */
static inline void new_npuzzle(NPuzzleSearchProblem* o, IDataType* t, size_t n) {
  o->n = n;
  o->count = n * n;
  o->max_depth = (n * n) * DEPTH_FACTOR;
  o->isp.state_expand = (ExpandState)npuzzle_expand;
  o->isp.state_heuristic = (Heuristic)npuzzle_heuristic;
  o->isp.state_interface = t;
  t->size = sizeof(size_t) + o->count * sizeof(uint8_t) + o->max_depth / 4;
  t->offset = sizeof(size_t);
  t->key_size = o->count * sizeof(uint8_t);
  t->cmp_eq = (Compare)npuzzle_state_cmp_eq;
  t->cmp_l = (Compare)npuzzle_state_cmp_l;
  t->cmp_le = (Compare)npuzzle_state_cmp_le;
  t->swap = (Operate)npuzzle_state_swap;
  t->hash = (Calculate)npuzzle_state_hash;
}

static inline void gen_initial_state(void* dest, size_t n, ...) {
  size_t size = sizeof(size_t) + (n * n) * sizeof(uint8_t) + ((n * n) * DEPTH_FACTOR) / 4;
  size_t count = n * n;
  uint8_t empty_piece = count - 1;
  memset(dest, 0, size);
  uint8_t* pieces = add_offset(dest, sizeof(size_t));
  // Prepare vargs.
  va_list vargs;
  va_start(vargs, count);
  // Get parameters.
  for(size_t i = 0; i < count; i++) {
    int piece = va_arg(vargs, int);
    if(piece == 0) {
      piece = empty_piece;
    } else {
      piece = piece - 1;
    }
    pieces[i] = piece;
  }
  va_end(vargs);
}

static inline void print_solution(const NPuzzleSearchProblem* o, const IDataType* t, const void* state) {
  uint8_t* pieces = add_offset(state, sizeof(size_t));
  size_t history_size = *((size_t*)state);
  HistoryCell* history = add_offset(pieces, t->key_size);
  printf("{");
  for(size_t i = 0; i < o->count; i++) {
    if(i == (o->count - 1)) {
      printf("%u", pieces[i]);
    } else {
      printf("%u, ", pieces[i]);
    }
  }
  printf("}\n");
  printf("Depth: %u\n", (uint32_t)history_size);
  for(size_t i = 0; i < history_size; i++) {
    size_t cell_index = i / 4;
    size_t cell_subindex = i % 4;
    switch(cell_subindex) {
      case 0:
        printf("%s\n", HISTORY2STR[history[cell_index].first]);
        break;
      case 1:
        printf("%s\n", HISTORY2STR[history[cell_index].second]);
        break;
      case 2:
        printf("%s\n", HISTORY2STR[history[cell_index].third]);
        break;
      case 3:
        printf("%s\n", HISTORY2STR[history[cell_index].fourth]);
        break;
      default:
        // Unreachable state.
        abort();
        break;
    }
  }
}

#endif /*TEST_AI_SEARCH_H*/