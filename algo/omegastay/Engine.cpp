#include <bits/stdc++.h>
#include "Constants.h"
#include "Engine.h"
#include "MCTS.h"
#include "Random.h"
#include "Store.h"
#include "Utility.h"
using namespace std;

#define REPL_ALL_CELL(x, y)          \
  for (int x = 0; x < Store::M; ++x) \
    for (int y = 0; y < Store::N; ++y)

State rootState;
vector<Position> shieldPos;

// Initialize store (distNoShield, distWithShield, etc.)
void initializeStore() {
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
      if (rootState.at[sx][sy] == DANGER_CELL) continue;

      dist(sx, sy) = 0;
      q.emplace(sx, sy);
      while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        for (int k = 0; k < NUM_MOVES; ++k) {
          int nx = x + dx[k];
          int ny = y + dy[k];
          if (isValidPos(nx, ny) && rootState.at[nx][ny] != DANGER_CELL && dist(nx, ny) == INF) {
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
        for (int k = 0; k < NUM_MOVES; ++k) {
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

  // Calculate numLegalMovesNoShield/numLegalMovesWithShield/isLegalMoveNoShield/isLegalMoveWithShield
  {
    memset(Store::numLegalMovesNoShield, 0, sizeof(Store::numLegalMovesNoShield));
    memset(Store::numLegalMovesWithShield, 0, sizeof(Store::numLegalMovesWithShield));

    memset(Store::isLegalMoveNoShield, 0, sizeof(Store::isLegalMoveNoShield));
    memset(Store::isLegalMoveWithShield, 0, sizeof(Store::isLegalMoveWithShield));

    REPL_ALL_CELL(x, y) {
      for (int k = 0; k < NUM_MOVES; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];

        if (isValidPos(nx, ny)) {
          ++Store::numLegalMovesWithShield[x][y];
          Store::isLegalMoveWithShield[x][y][k] = true;

          if (rootState.at[nx][ny] != DANGER_CELL) {
            ++Store::numLegalMovesNoShield[x][y];
            Store::isLegalMoveNoShield[x][y][k] = true;
          }
        }
      }
    }
  }

  // Initialize other fields of store
  // M, N, K, HALF_K is already set in main::readInput()
  Store::currentTurn = 1;
  Store::pastStates.push_back(rootState);

  // Save store
  Store::save();
}

void handleFirstTurn() {
  // Find starting position
  // For now, we just find the nearest-to-shield empty cell
  // FIXME: better algorithm
  {
    int minDistToShield = INF;
    vector<array<int, 2>> candidates;
    REPL_ALL_CELL(x, y) {
      if (rootState.at[x][y] == EMPTY_CELL) {
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
    rootState.eliminated[0] = Store::pastStates.back().eliminated[0];
    rootState.eliminated[1] = Store::pastStates.back().eliminated[1];
    rootState.gold[1] = Store::pastStates.back().gold[1];
    rootState.hasShield[1] = Store::pastStates.back().hasShield[1];

    auto [x, y] = rootState.pos[1];
    int prevCell = Store::pastStates.back().at[x][y];
    if (prevCell == SHIELD_CELL) {
      rootState.hasShield[1] = true;
    } else if (prevCell == DANGER_CELL) {
      if (!rootState.hasShield[1]) {
        rootState.eliminated[1] = true;
      }
    } else if (prevCell != EMPTY_CELL) {
      rootState.gold[1] += prevCell;
    }
  }

  // Update store with new data
  Store::currentTurn += 1;
  Store::pastStates.push_back(rootState);

  // Save store
  Store::save();

  // Find best move using Monte Carlo tree search
  MonteCarloTreeSearch mtcs(rootState);
  int lastMove = -1;
#if defined(LIMIT_NUMBER_OF_ITERATIONS) || defined(ENABLE_DEBUG_MODE)
  int count = 0;
#endif
#ifdef LIMIT_NUMBER_OF_ITERATIONS
  while (count < MAX_ITERATIONS) {
    ++count;
#else
  while (true) {
#endif
    auto move = mtcs.findBestMove(MTCS_ITERATIONS);
    if (move != lastMove) {
      lastMove = move;
      int x = rootState.pos[0].x + dx[move];
      int y = rootState.pos[0].y + dy[move];
      printFinalMove(x, y);

#ifdef ENABLE_DEBUG_MODE
      cerr << "Found new best move " << x + 1 << ' ' << y + 1 << ' ' << count << endl;
#endif
    }
  }
}
