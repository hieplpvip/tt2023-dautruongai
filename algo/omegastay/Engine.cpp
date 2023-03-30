#include <bits/stdc++.h>
#include "Engine.h"
#include "Store.h"
#include "Utility.h"
#include "Constants.h"
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
  // Calculate distNoShield using BFS
  {
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
  }

  // Calculate distWithShield using BFS
  {
#define dist(x, y) Store::distWithShield[sx][sy][x][y]
    queue<Position> q;
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

  // Update store with new data
  Store::currentTurn += 1;
  Store::pastStates.push_back(rootState);

  // TODO: update opponent's shield/gold

  // Save store
  Store::load();
}
