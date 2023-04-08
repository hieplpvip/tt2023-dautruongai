#include "Heuristic.h"
#include "Store.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#define sqr(a) ((a) * (a))
#define dist(player, u, v) Store::dist[state.hasShield[player]][state.pos[player].x][state.pos[player].y][u][v]

namespace Heuristic {
  std::vector<std::vector<score_t>> GetHeatMap(State state, PlayerEnum player) {
    // Note that table is 1-indexed
    std::vector<std::vector<score_t>> table(Store::M + 2, std::vector<score_t>(Store::N + 2, 0));

    if (state.turnLeft > Store::HALF_K && state.turnLeft - Store::HALF_K <= dist(player, Store::M / 2, Store::N / 2)) {
      state.at[Store::M / 2][Store::N / 2] = std::max(1, abs(state.gold[0] - state.gold[1]) * 3 / 4);
    }

    REPL_ALL_CELL(x, y) {
      score_t val;

      // Get the raw value of the cell
      if (state.at[x][y] == DANGER_CELL) {
        val = 0;
      } else if (state.at[x][y] == SHIELD_CELL) {
        // In late game, don't pick up shield
        if (state.hasShield[player] || Store::gamePhase == GamePhaseEnum::LATE_GAME) {
          val = 0;
        } else {
          val = SHIELD_VALUE;
        }
      } else {
        val = state.at[x][y];
      }

      // Reduce gold value if enemy is too close
      if (dist(1 - player, x, y) <= std::min(ENEMY_RADIUS, dist(player, x, y))) {
        val = 2.f / 3 * val;
      }

      // If cannot reach the cell in turn left, ignore
      if (dist(player, x, y) >= state.turnLeft) {
        val = 0;
      }

      val = Evaluate(val, dist(player, x, y));

      // accumulate table
      int i1 = std::max(1, x + 1 - HEAT_RADIUS), j1 = std::max(1, y + 1 - HEAT_RADIUS);
      int i2 = std::min(Store::M, x + 1 + HEAT_RADIUS), j2 = std::min(Store::N, y + 1 + HEAT_RADIUS);
      table[i1][j1] += val;
      table[i1][j2 + 1] -= val;
      table[i2 + 1][j1] -= val;
      table[i2 + 1][j2 + 1] += val;
    }

    REPL_ALL_CELL(x, y) {
      table[x + 1][y + 1] += table[x + 1][y] + table[x][y + 1] - table[x][y];
    }

    return table;
  }

  std::vector<std::vector<score_t>> GetHeatMap(State state) {
    // Note that the table is indexed from 1
    std::vector<std::vector<score_t>> table(Store::M + 2, std::vector<score_t>(Store::N + 2, 0));

    REPL_ALL_CELL(x, y) {
      score_t val;

      // Get the raw value of the cell
      if (state.at[x][y] == DANGER_CELL) {
        val = 0;
      } else if (state.at[x][y] == SHIELD_CELL) {
        val = SHIELD_VALUE;
      } else {
        val = state.at[x][y];
      }

      // accumulate table
      int i1 = std::max(1, x + 1 - HEAT_RADIUS), j1 = std::max(1, y + 1 - HEAT_RADIUS);
      int i2 = std::min(Store::M, x + 1 + HEAT_RADIUS), j2 = std::min(Store::N, y + 1 + HEAT_RADIUS);
      table[i1][j1] += val;
      table[i1][j2 + 1] -= val;
      table[i2 + 1][j1] -= val;
      table[i2 + 1][j2 + 1] += val;
    }

    REPL_ALL_CELL(x, y) {
      table[x + 1][y + 1] += table[x + 1][y] + table[x][y + 1] - table[x][y];
    }

    return table;
  }

  std::vector<std::pair<score_t, Position>> GetCandidates(State state) {
    // Note that table is 1-indexed
    std::vector<std::vector<score_t>> table = GetHeatMap(state);

    std::vector<std::pair<score_t, Position>> candidates;

    REPL_ALL_CELL(x, y) {
      if (state.at[x][y] != EMPTY_CELL) {
        continue;
      }
      candidates.emplace_back(table[x + 1][y + 1], Position(x, y));
    }

    sort(candidates.rbegin(), candidates.rend());
    candidates.resize(std::min((int)candidates.size(), NUM_CANDS));
    return candidates;
  }

  score_t GetHighestHeat(State state, PlayerEnum player) {
    // Note that table is 1-indexed
    std::vector<std::vector<score_t>> table = GetHeatMap(state, player);

    score_t bestScore = -INF;
    REPL_ALL_CELL(x, y) {
      bestScore = std::max(bestScore, table[x + 1][y + 1]);
    }

    return bestScore;
  }

  score_t Evaluate(double gold, int distance) {
    score_t val = 0;

    if (Store::gamePhase == GamePhaseEnum::EARLY_GAME || Store::gamePhase == GamePhaseEnum::MID_GAME) {
      // Heuristic: reduce the value of the cell if it is far from the player
      val = gold * (BONUS - sqrt(distance));
    } else {
      // Heuristic: increase the gold value by squaring
      val = BONUS * sqr(gold + 1) / sqrt(distance + 1);
    }

    return std::max(val, (double)0);
  }
}
