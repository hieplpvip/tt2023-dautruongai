#ifndef CONSTANTS_H
#define CONSTANTS_H

// Take shield immediately if it standing next to one and not having shield yet.
// Comment out to disable
// #define TAKE_SHIELD_IMMEDIATELY

// Comment out to enable assert
#define NDEBUG

// Comment out to disable debug mode
// #define ENABLE_DEBUG_MODE

// Comment out to run until timed out
#define LIMIT_NUMBER_OF_ITERATIONS
#define MAX_ITERATIONS 10

// Least margin to be considered as win
#define WIN_MARGIN 5

// Number of nodes to be preallocated to avoid overhead of new
#define NUMBER_OF_PREALLOCATED_NODES 500000

#include <cmath>

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;

// Number of iterations of MCTS in one run
constexpr int MTCS_ITERATIONS = 1000;

// Number of visits until a node can be selected using UCT instead of randomly
constexpr int MTCS_MIN_VISITS = 10;

// C in the UCT formula
const double MCTS_C = sqrt(2);

/*
 * All possible moves.
 * Guaranteed that UP ^ DOWN = 1, LEFT ^ RIGHT = 1
 */
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
