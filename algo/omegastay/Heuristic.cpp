#include "Heuristic.h"
#include "Store.h"
#include <algorithm>
#include <cmath>
#include <cstring>
using std::min, std::max;

#define sqr(a) ((a) * (a))
#define dist(player, u, v) Store::dist[state.hasShield[player]][state.pos[player].x][state.pos[player].y][u][v]

constexpr int SHIELD_VALUE = 100;
constexpr int NUM_CANDS = 100;
constexpr int BONUS = 10;
constexpr int HEAT_RADIUS = 2;   // If the radius is too large, it will detect the gold in sparse areas
constexpr int ENEMY_RADIUS = 2;  // If radius too large, it will miss a lot of gold, otherwise it may come close to enemy

namespace Heuristic {
  std::vector<std::vector<double>> GetHeatMap(State& state, int player) {
    // Note that table is 1-indexed
    std::vector<std::vector<double>> table(Store::M + 2, std::vector<double>(Store::N + 2, 0));

    if (state.turnLeft > Store::HALF_K && state.turnLeft - Store::HALF_K <= dist(player, Store::M / 2, Store::N / 2)) {
      // Still in the first half of the game
      // Approximate treasure value
      state.at[Store::M / 2][Store::N / 2] = std::max(1, abs(state.gold[0] - state.gold[1]) * 3 / 4);
    }

    REPL_ALL_CELL(x, y) {
      double val;

      // Get the raw value of the cell
      if (state.at[x][y] == DANGER_CELL) {
        val = 0;
      } else if (state.at[x][y] == SHIELD_CELL) {
        val = state.hasShield[player] ? 0 : SHIELD_CELL;
      } else {
        val = state.at[x][y];
      }

      // Reduce gold value if enemy is too close
      if (dist(1 - player, x, y) <= std::min(ENEMY_RADIUS, dist(player, x, y))) {
        val = 1.f / 3 * val;
      }

      // Heuristic: increase the gold value by squaring
      val = BONUS * sqr(val + 1) / sqrt(dist(player, x, y) + 1);

      // Note that x and y are 0-indexed, but score is 1-indexed
      // Add val to all cells between (i1, j1) and (i2, j2)
      int i1 = std::max(1, x + 1 - HEAT_RADIUS), j1 = std::max(1, y + 1 - HEAT_RADIUS);
      int i2 = std::min(Store::M, x + 1 + HEAT_RADIUS), j2 = std::min(Store::N, y + 1 + HEAT_RADIUS);
      table[i1][j1] += val;
      table[i1][j2 + 1] -= val;
      table[i2 + 1][j1] -= val;
      table[i2 + 1][j2 + 1] += val;
    }

    // Accumulate table
    REPL_ALL_CELL(x, y) {
      table[x + 1][y + 1] += table[x + 1][y] + table[x][y + 1] - table[x][y];
    }

    if (state.turnLeft > Store::HALF_K) {
      // Reset center to empty
      state.at[Store::M / 2][Store::N / 2] = EMPTY_CELL;
    }

    return table;
  }

  std::vector<std::vector<double>> GetHeatMap(State& state) {
    // Note that the table is indexed from 1
    std::vector<std::vector<double>> table(Store::M + 2, std::vector<double>(Store::N + 2, 0));

    REPL_ALL_CELL(x, y) {
      double val;

      // Get the raw value of the cell
      if (state.at[x][y] == DANGER_CELL) {
        val = 0;
      } else if (state.at[x][y] == SHIELD_CELL) {
        val = SHIELD_VALUE;
      } else {
        val = state.at[x][y];
      }

      // Note that x and y are 0-indexed, but score is 1-indexed
      // Add val to all cells between (i1, j1) and (i2, j2)
      int i1 = std::max(1, x + 1 - HEAT_RADIUS), j1 = std::max(1, y + 1 - HEAT_RADIUS);
      int i2 = std::min(Store::M, x + 1 + HEAT_RADIUS), j2 = std::min(Store::N, y + 1 + HEAT_RADIUS);
      table[i1][j1] += val;
      table[i1][j2 + 1] -= val;
      table[i2 + 1][j1] -= val;
      table[i2 + 1][j2 + 1] += val;
    }

    // Accumulate table
    REPL_ALL_CELL(x, y) {
      table[x + 1][y + 1] += table[x + 1][y] + table[x][y + 1] - table[x][y];
    }

    return table;
  }

  std::vector<std::pair<double, Position>> GetCandidates(State& state) {
    // Note that table is 1-indexed
    std::vector<std::vector<double>> table = GetHeatMap(state);

    std::vector<std::pair<double, Position>> candidates;

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

  double GetHighestHeat(State& state, int player) {
    // Note that table is 1-indexed
    std::vector<std::vector<double>> table = GetHeatMap(state, player);

    double bestScore = -INF;
    REPL_ALL_CELL(x, y) {
      bestScore = std::max(bestScore, table[x + 1][y + 1]);
    }

    return bestScore;
  }

  Position GetHighestHeatPosition(State& state, int player) {
    // Note that table is 1-indexed
    std::vector<std::vector<double>> table = GetHeatMap(state, player);

    double bestScore = -INF;
    int bestX = -1, bestY = -1;
    REPL_ALL_CELL(x, y) {
      if (table[x + 1][y + 1] > bestScore) {
        bestScore = table[x + 1][y + 1];
        bestX = x;
        bestY = y;
      }
    }

    return {bestX, bestY};
  }
}
