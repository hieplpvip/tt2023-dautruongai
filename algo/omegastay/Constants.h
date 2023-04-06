#ifndef CONSTANTS_H
#define CONSTANTS_H

// When this is defined, a draw will get a score of -1 instead of 0.
// In other words, the AI will try to win at all costs.
// Comment out to disable
#define DRAW_IS_UNACCEPTABLE

// Comment out to enable assert
#define NDEBUG

// Comment out to disable debug mode
// #define ENABLE_DEBUG_MODE

// Comment out to run until timed out
#define LIMIT_NUMBER_OF_ITERATIONS
#define MAX_ITERATIONS 400

// Number of nodes to be preallocated to avoid overhead of new
#define NUMBER_OF_PREALLOCATED_NODES 1000000

#include <cmath>

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;

// Number of iterations of MCTS in one run
constexpr int MTCS_ITERATIONS = 1000;

// Number of visits until a node can be selected using UCT instead of randomly
constexpr int MTCS_MIN_VISITS = 5;

/** C in the UCT formula */
const double MCTS_C = sqrt(2);

enum MoveEnum {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
};

constexpr int NUM_MOVES = 4;
constexpr int dx[] = {-1, +1, 0, 0};
constexpr int dy[] = {0, 0, -1, +1};

#endif
