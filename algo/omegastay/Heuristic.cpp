#include "Heuristic.h"
#include "Store.h"
#include <algorithm>
#include <cmath>
#include <cstring>

#define sqr(a) ((a) * (a))

constexpr int HEAT_RADIUS = 2;  // If the radius is too large, it will detect the gold in sparse areas
constexpr int SHIELD_VALUE = 7;

namespace Heuristic {
  // Preallocated heat map for faster computation
  // Note that it is 1-indexed
  static double table[20][20];

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
}
