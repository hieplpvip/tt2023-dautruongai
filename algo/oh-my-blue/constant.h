#ifndef CONSTANT_H
#define CONSTANT_H

constexpr int INF = 1e9;

constexpr int EMPTY_CELL = 0;
constexpr int DANGER_CELL = 101;
constexpr int SHIELD_CELL = 102;

constexpr int MAX_DEPTH = 8;
constexpr int BRANCHING_FACTOR = 3;
constexpr int MAGNET_RADIUS = 1;
constexpr float MAGNET_RATE = 0.4;
constexpr float NEIGHBOR_RATE = 0.6;  // If this value increases, the agent will ignore the nearby cells and focus on the areas with a lot of gold
constexpr int HEAT_RADIUS = 2;        // If the radius is too large, it will detect the gold in sparse areas
constexpr int BASE_GOLD = 50;

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