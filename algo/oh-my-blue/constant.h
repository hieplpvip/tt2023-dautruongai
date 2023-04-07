#ifndef CONSTANT_H
#define CONSTANT_H

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;

constexpr int MAX_DEPTH = 14;
constexpr int HEAT_RADIUS = 2;  // If the radius is too large, it will detect the gold in sparse areas
constexpr int NUM_CANDS = 50;
constexpr int BONUS = 10;
constexpr int ENEMY_RADIUS = 2;  // If radius too large, it will miss a lot of gold, otherwise it may come close to enemy

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

constexpr int NUM_MOVES = 4;
constexpr int dx[] = {-1, +1, 0, 0};
constexpr int dy[] = {0, 0, -1, +1};

#endif  // CONSTANT_H