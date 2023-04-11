#include "Heuristic.h"
#include "Store.h"
#include <algorithm>
#include <cmath>
#include <cstring>

#define sqr(a) ((a) * (a))
#define dist(player, u, v) Store::dist[state.hasShield[player]][state.pos[player].x][state.pos[player].y][u][v]

constexpr int HEAT_RADIUS = 2;   // If the radius is too large, it will detect the gold in sparse areas
constexpr int ENEMY_RADIUS = 3;  // If radius too large, it will miss a lot of gold, otherwise it may come close to enemy
constexpr int SHIELD_VALUE = 7;
constexpr int BONUS = 10;

namespace Heuristic {
  // Preallocated heat map for faster computation
  // Note that it is 1-indexed
  static double table[20][20];

  /*
   * Generate heat map for a state according to player.
   * For each cell(x, y) it will compute the heat in the range of HEAT_RADIUS.
   */
  void GetHeatMap(State& state, PlayerEnum player) {
    // Reset table to zero
    memset(table, 0, sizeof(table));

    if (state.turnLeft > Store::HALF_K && state.turnLeft - Store::HALF_K <= dist(player, Store::M / 2, Store::N / 2)) {
      // Still in the first half of the game
      // Approximate treasure value
      state.at[Store::M / 2][Store::N / 2] = std::max(1, abs(state.gold[0] - state.gold[1]) * 3 / 4);
    }

    REPL_ALL_CELL(x, y) {
      score_t val;

      // Get the raw value of the cell
      if (state.at[x][y] == DANGER_CELL) {
        val = 0;
      } else if (state.at[x][y] == SHIELD_CELL) {
        // In late game, don't pick up shield
        // if (player == PlayerEnum::ENEMY || state.hasShield[player] || Store::gamePhase == GamePhaseEnum::LATE_GAME) {
        //   val = 0;
        // } else {
        //   val = SHIELD_VALUE;
        // }

        val = 0;
      } else {
        val = state.at[x][y];
      }

      // Reduce gold value if enemy is too close
      // if (dist(1 - player, x, y) <= std::min(ENEMY_RADIUS, dist(player, x, y))) {
      //   val = 2.f / 3 * val;
      // }
      if (dist(1 - player, x, y) <= dist(player, x, y)) {
        val = 1.f / 3 * val;
      }

      // If cannot reach the cell in turn left, ignore
      if (dist(player, x, y) >= state.turnLeft) {
        val = 0;
      }

      val = Evaluate(val, dist(player, x, y));

      // Note that x and y are 0-indexed, but table is 1-indexed
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
  }

  /*
   * Generate heat map for a state without player.
   * Use for initial state.
   */
  void GetHeatMap(State& state) {
    // Reset table to zero
    memset(table, 0, sizeof(table));

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

      // Note that x and y are 0-indexed, but table is 1-indexed
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
  }

  std::vector<std::pair<score_t, Position>> GetCandidates(State& state, int numCandidates) {
    GetHeatMap(state);

    std::vector<std::pair<score_t, Position>> candidates;
    REPL_ALL_CELL(x, y) {
      if (state.at[x][y] != EMPTY_CELL) {
        continue;
      }
      bool adjacentToShieldOrGold = false;
      for (int k = 0; k < NUM_MOVES; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];
        if (isValidPos(nx, ny) && (state.at[nx][ny] == SHIELD_CELL || (state.at[nx][ny] != DANGER_CELL && state.at[nx][ny] != EMPTY_CELL))) {
          adjacentToShieldOrGold = true;
          break;
        }
      }
      if (adjacentToShieldOrGold) {
        candidates.emplace_back(table[x + 1][y + 1], Position(x, y));
      }
    }

    sort(candidates.rbegin(), candidates.rend());
    candidates.resize(std::min((int)candidates.size(), numCandidates));
    return candidates;
  }

  score_t GetHighestHeat(State& state, PlayerEnum player) {
    GetHeatMap(state, player);

    score_t bestScore = -INF;
    REPL_ALL_CELL(x, y) {
      bestScore = std::max(bestScore, table[x + 1][y + 1]);
    }
    return bestScore;
  }

  score_t Evaluate(double gold, int distance) {
    // score_t val = 0;

    // if (Store::gamePhase == GamePhaseEnum::EARLY_GAME || Store::gamePhase == GamePhaseEnum::MID_GAME) {
    //   // Heuristic: reduce the value of the cell if it is far from the player
    //   val = gold * (BONUS - sqrt(distance));
    // } else {
    //   // Heuristic: increase the gold value by squaring
    //   val = BONUS * sqr(gold + 1) / sqrt(distance + 1);
    // }

    // return std::max(val, (double)0);

    return sqr(gold + 1) / sqrt(distance + 1);
    // return gold / (distance + 1);
  }
}
