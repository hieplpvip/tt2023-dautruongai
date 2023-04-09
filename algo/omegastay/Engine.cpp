#include <bits/stdc++.h>
#include "Constants.h"
#include "Engine.h"
#include "Heuristic.h"
#include "MCTS.h"
#include "Random.h"
#include "Store.h"
#include "Utility.h"
using namespace std;

vector<Position> shieldPos;

namespace Ignition {
  void findStartingPositionOld() {
    // Find starting position
    // For now, we just find the nearest-to-shield empty cell
    int minDistToShield = INF;
    vector<array<int, 2>> candidates;
    REPL_ALL_CELL(x, y) {
      if (rootState.at[x][y] == EMPTY_CELL) {
        for (auto [shieldX, shieldY] : shieldPos) {
          int dist = Store::dist[0][x][y][shieldX][shieldY];
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

  void findStartingPosition() {
    // vector<tuple<double, int, int>> cand;

    // // Place opponent ship at some danger cell to ignore opponent in heuristic
    // rootState.pos[1] = {0, 0};
    // REPL_ALL_CELL(x, y) {
    //   if (rootState.at[x][y] == DANGER_CELL) {
    //     rootState.pos[1] = {x, y};
    //     break;
    //   }
    // }

    // // Evaluate all empty cells
    // REPL_ALL_CELL(x, y) {
    //   if (rootState.at[x][y] != EMPTY_CELL) {
    //     continue;
    //   }
    //   rootState.pos[0] = {x, y};
    //   auto heat = Heuristic::GetHighestHeat(rootState, 0);
    //   cand.emplace_back(heat, x, y);
    // }

    // // Restore positions
    // rootState.pos[0] = rootState.pos[1] = {-1, -1};

    // // Sort decreasingly by gold, and choose randomly among top 3
    // int k = 3;
    // sort(cand.rbegin(), cand.rend());
    // while (k && get<0>(cand[k - 1]) <= -INF + 5) --k;
    // auto [_, x, y] = cand[Random::rand(k)];
    // printFinalMove(x, y);

    findStartingPositionOld();
  }
}

namespace TurboFan {
  void findNextMove() {
#ifdef TAKE_SHIELD_IMMEDIATELY
    if (!rootState.hasShield[0] && rootState.gold[0] > rootState.gold[1]) {
      // Take shield if not taken yet, but only when we are having more gold
      auto [x, y] = rootState.pos[0];
      for (int k = 0; k < 4; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];
        if (isValidPos(nx, ny) && rootState.at[nx][ny] == SHIELD_CELL) {
#ifdef ENABLE_DEBUG_MODE
          cerr << "Take shield " << nx + 1 << ' ' << ny + 1 << endl;
#endif
          printFinalMove(nx, ny);
          return;
        }
      }
    }
#endif

#ifdef TAKE_GOLD_IMMEDIATELY
    {
      // Find adjacent gold cells
      int gold = -1, best_x = -1, best_y = -1;
      auto [x, y] = rootState.pos[0];
      for (int k = 0; k < 4; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];
        if (isValidPos(nx, ny) && rootState.at[nx][ny] != SHIELD_CELL && rootState.at[nx][ny] != DANGER_CELL && rootState.at[nx][ny] > gold) {
          gold = rootState.at[nx][ny];
          best_x = nx;
          best_y = ny;
        }
      }

      if ((Store::gamePhase == LATE_GAME && gold >= 3) || (rootState.turnLeft <= 5 && gold > 0)) {
#ifdef ENABLE_DEBUG_MODE
        cerr << "Take " << gold << " gold " << best_x + 1 << ' ' << best_y + 1 << endl;
#endif
        printFinalMove(best_x, best_y);
        return;
      }
    }
#endif

    // Find best move using Monte Carlo tree search
    MonteCarloTreeSearch mcts(rootState);
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
      auto move = mcts.findBestMove(MCTS_ITERATIONS);
      if (move != lastMove) {
        lastMove = move;
        int x = rootState.pos[0].x + dx[move];
        int y = rootState.pos[0].y + dy[move];
        printFinalMove(x, y);

#ifdef ENABLE_DEBUG_MODE
        cerr << "Found new best move " << x + 1 << ' ' << y + 1 << ' ' << count << endl;
        mcts.printStats();
#endif
      }
    }
  }
}
