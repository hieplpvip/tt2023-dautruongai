#ifndef CONSTANTS_H
#define CONSTANTS_H

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
