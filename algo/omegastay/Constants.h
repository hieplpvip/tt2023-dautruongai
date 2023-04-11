#ifndef CONSTANTS_H
#define CONSTANTS_H

// Take shield immediately if it standing next to one and not having shield yet,
// but only when we are having more gold or opponent is not standing next to shield
// Comment out to disable
#define TAKE_SHIELD_IMMEDIATELY

// Take gold immediately if in late game
// Comment out to disable
#define TAKE_GOLD_IMMEDIATELY

// Comment out to enable assert
#define NDEBUG

// Comment out to disable logging
#define ENABLE_LOGGING

#include <cmath>

using score_t = double;

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;

// Determine when to switch to late game
constexpr int THRESHOLD_MID_GAME = 50;

// Determine when to switch to mid game
constexpr int THRESHOLD_EARLY_GAME = 15;

// If number of turns left is at most this value, Negamax is used instead of MCTS
// The effective maximum depth of Negamax would be 2 * NEGAMAX_MAX_TURN_LEFT
constexpr int NEGAMAX_MAX_TURN_LEFT = 10;

// Number of nodes to be preallocated to avoid overhead of new
constexpr int NUMBER_OF_PREALLOCATED_NODES = 500000;

// Number of MCTS iterations
constexpr int MCTS_MAX_ITERATIONS = 250;

// Number of iterations of MCTS in one run
constexpr int MCTS_NUM_SIMULATIONS = 1000;

// Number of visits until a node can be selected using UCT instead of randomly
constexpr int MCTS_MIN_VISITS = 10;

// C in the UCT formula
const double MCTS_C = sqrt(2);

enum PlayerEnum {
  ME = 0,
  ENEMY = 1,
};

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

enum GamePhaseEnum {
  EARLY_GAME = 0,
  MID_GAME = 1,
  LATE_GAME = 2,
};

constexpr int NUM_MOVES = 4;
constexpr int dx[] = {-1, +1, 0, 0};
constexpr int dy[] = {0, 0, -1, +1};

#endif
