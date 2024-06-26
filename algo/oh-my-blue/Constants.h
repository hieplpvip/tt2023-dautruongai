#ifndef CONSTANTS_H
#define CONSTANTS_H

// Comment out to disable logging
// #define ENABLE_LOGGING

using score_t = double;

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;

// Determine when to switch to late game
constexpr int THRESHOLD_MID_GAME = 50;

// Determine when to switch to mid game
constexpr int THRESHOLD_EARLY_GAME = 15;

// Maximum search depth of Minimax
// A value of x means each player can make x / 2 moves
constexpr int MINIMAX_MAX_DEPTH = 18;

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
