#include <bits/stdc++.h>
#include "Constants.h"
#include "Engine.h"
#include "Store.h"
#include "Utility.h"
using namespace std;

#define REPL_ALL_CELL(x, y)          \
  for (int x = 0; x < Store::M; ++x) \
    for (int y = 0; y < Store::N; ++y)

State rootState;
vector<Position> shieldPos;

inline bool isValidPos(int x, int y) {
  return 0 <= x && x < Store::M && 0 <= y && y < Store::N;
}

/*
 * Initialize store (distNoShield, distWithShield, currentTurn, etc.) and
 * find starting position
 */
void handleFirstTurn() {
  // Calculate distNoShield/distWithShield using BFS
  {
    REPL_ALL_CELL(x1, y1) {
      REPL_ALL_CELL(x2, y2) {
        Store::distNoShield[x1][y1][x2][y2] = Store::distWithShield[x1][y1][x2][y2] = INF;
      }
    }

#define dist(x, y) Store::distNoShield[sx][sy][x][y]
    queue<Position> q;
    REPL_ALL_CELL(sx, sy) {
      if (rootState.cell[sx][sy] == DANGER_CELL) continue;

      dist(sx, sy) = 0;
      q.emplace(sx, sy);
      while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        for (int k = 0; k < 4; ++k) {
          int nx = x + dx[k];
          int ny = y + dy[k];
          if (isValidPos(nx, ny) && rootState.cell[nx][ny] != DANGER_CELL && dist(nx, ny) == INF) {
            dist(nx, ny) = dist(x, y) + 1;
            q.emplace(nx, ny);
          }
        }
      }
    }
#undef dist

#define dist(x, y) Store::distWithShield[sx][sy][x][y]
    REPL_ALL_CELL(sx, sy) {
      dist(sx, sy) = 0;
      q.emplace(sx, sy);
      while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        for (int k = 0; k < 4; ++k) {
          int nx = x + dx[k];
          int ny = y + dy[k];
          if (isValidPos(nx, ny) && dist(nx, ny) == INF) {
            dist(nx, ny) = dist(x, y) + 1;
            q.emplace(nx, ny);
          }
        }
      }
    }
#undef dist
  }

  // Initialize other field of store
  Store::currentTurn = 1;
  Store::pastStates.push_back(rootState);

  // Save store
  Store::save();

  // Find starting position
  // For now, we just find the nearest-to-shield empty cell
  // FIXME: better algorithm
  {
    int minDistToShield = INF;
    vector<array<int, 2>> candidates;
    REPL_ALL_CELL(x, y) {
      if (rootState.cell[x][y] == EMPTY_CELL) {
        for (auto [shieldX, shieldY] : shieldPos) {
          int dist = Store::distNoShield[x][y][shieldX][shieldY];
          if (dist < minDistToShield) {
            minDistToShield = dist;
            candidates.clear();
            candidates.push_back({x, y});
          } else if (dist == minDistToShield) {
            candidates.push_back({x, y});
          }
        }
      }
    }
    assert(!candidates.empty());

    // Randomly choose one of the candidates
    int idx = Random::rand(candidates.size());
    auto [x, y] = candidates[idx];
    printFinalMove(x, y);
  }
}

void handleOtherTurns() {
  // Load store
  Store::load();

  // Update opponent's shield/gold
  {
    rootState.gold[1] = Store::pastStates.back().gold[1];
    rootState.hasShield[1] = Store::pastStates.back().hasShield[1];

    auto [x, y] = rootState.pos[1];
    int prevCell = Store::pastStates.back().cell[x][y];
    if (prevCell == SHIELD_CELL) {
      rootState.hasShield[1] = true;
    } else if (prevCell != DANGER_CELL && prevCell != EMPTY_CELL) {
      rootState.gold[1] += prevCell;
    }
  }
  cout << "Opponent's gold: " << rootState.gold[1] << endl;
  cout << "Opponent's shield: " << rootState.hasShield[1] << endl;

  // Update store with new data
  Store::currentTurn += 1;
  Store::pastStates.push_back(rootState);

  // Save store
  Store::save();

  int gold_x = -1, gold_y = -1, gold_max = 0;
  int shield_x = -1, shield_y = -1;
  vector<Position> free;

  for (int k = 0; k < 4; ++k) {
    int u = rootState.pos[0].x + dx[k];
    int v = rootState.pos[0].y + dy[k];
    if (!isValidPos(u, v)) continue;

    if (rootState.cell[u][v] == EMPTY_CELL) {
      free.emplace_back(u, v);
    } else if (rootState.cell[u][v] == DANGER_CELL) {
      if (rootState.hasShield[0]) {
        free.emplace_back(u, v);
      }
    } else if (rootState.cell[u][v] == SHIELD_CELL) {
      shield_x = u;
      shield_y = v;
    } else if (rootState.cell[u][v] > gold_max) {
      gold_x = u;
      gold_y = v;
      gold_max = rootState.cell[u][v];
    }
  }

  if (shield_x != -1) {
    // Take the fucking shield
    printFinalMove(shield_x, shield_y);
    return;
  }

  if (gold_x != -1) {
    // Take gold
    printFinalMove(gold_x, gold_y);
    return;
  }

  if (!free.empty()) {
    auto [x, y] = free[Random::rand(free.size())];
    printFinalMove(x, y);
    return;
  }

  printFinalMove(-1, -1);
}
