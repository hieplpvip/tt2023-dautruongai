#ifndef CONSTANT_H
#define CONSTANT_H

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;
constexpr int SHIELD_VALUE = 20;

constexpr int MAX_DEPTH = 12;
constexpr int HEAT_RADIUS = 2;  // If the radius is too large, it will detect the gold in sparse areas
constexpr int NUM_CANDS = 100;
constexpr int BONUS = 10;
constexpr int ENEMY_RADIUS = 2;           // If radius too large, it will miss a lot of gold, otherwise it may come close to enemy
constexpr int THRESHOLD_MID_GAME = 50;    // Determine when to switch to late game
constexpr int THRESHOLD_EARLY_GAME = 15;  // Determine when to switch to mid game

enum PlayerEnum {
  ME = 0,
  ENEMY = 1,
};

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

#endif  // CONSTANT_H
