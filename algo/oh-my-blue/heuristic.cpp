#include "heuristic.h"
#include "store.h"

#include <algorithm>
#include <cmath>

#define dist(player, x, y) Store::dist[state.hasShield[player]][state.pos[player].x][state.pos[player].y][x][y]

namespace Heuristic {
  std::vector<std::pair<score_t, MoveEnum>> GetBestMoves(State state, PlayerEnum player) {
    std::vector<std::vector<score_t>> table(Store::M + 2, std::vector<score_t>(Store::N + 2, 0));

    if (state.turnLeft > Store::HALF_K) {
      state.at[Store::M / 2 + 1][Store::N / 2 + 1] = sqr((state.gold[0] - state.gold[1]) * 3 / 4) / sqrt(std::max(1, state.turnLeft - Store::HALF_K));
    }

    REPL_ALL_CELL(x, y) {
      score_t val;

      // Get the raw value of the cell
      if (state.at[x][y] == DANGER_CELL) {
        val = 0;
      } else if (state.at[x][y] == SHIELD_CELL) {
        val = state.hasShield[player] ? 0 : SHIELD_CELL;
      } else {
        val = state.at[x][y];
      }

      // Add bonus for each adjacent gold
      for (int i = 0; i < NUM_MOVES; ++i) {
        int a = x + dx[i];
        int b = y + dy[i];
        if (isValidPos(a, b) && state.at[a][b] != DANGER_CELL && state.at[a][b] != SHIELD_CELL) {
          val += state.at[a][b] * NEIGHBOR_RATE;
        }
      }

      // Gold is reduced by 1/3 if it is in the range of the enemy
      if (dist(1 - player, x, y) <= dist(player, x, y)) {
        val = val * 2 / 3;
      }

      // Heuristic: reduce the value of the cell if it is far from the player
      // val = BASE_GOLD * val / sqrt(dist(player, x, y) + 1);
      val = BASE_GOLD * val - dist(player, x, y);

      // accumulate table
      int i1 = std::max(1, x + 1 - HEAT_RADIUS), j1 = std::max(1, y + 1 - HEAT_RADIUS);
      int i2 = std::min(Store::M, x + 1 + HEAT_RADIUS), j2 = std::min(Store::N, y + 1 + HEAT_RADIUS);
      table[i1][j1] += val;
      table[i1][j2 + 1] -= val;
      table[i2 + 1][j1] -= val;
      table[i2 + 1][j2 + 1] += val;
    }

    std::vector<std::pair<score_t, MoveEnum>> moves;
    for (int i = 0; i < NUM_MOVES; ++i) {
      moves.push_back({0, static_cast<MoveEnum>(i)});
    }

    REPL_ALL_CELL(x, y) {
      table[x + 1][y + 1] += table[x + 1][y] + table[x][y + 1] - table[x][y];

      // Find highest heat area according to direction
      if (x < state.pos[player].x) {
        moves[UP].first = std::max(moves[UP].first, table[x + 1][y + 1]);
      }
      if (x > state.pos[player].x) {
        moves[DOWN].first = std::max(moves[DOWN].first, table[x + 1][y + 1]);
      }
      if (y < state.pos[player].y) {
        moves[LEFT].first = std::max(moves[LEFT].first, table[x + 1][y + 1]);
      }
      if (y > state.pos[player].y) {
        moves[RIGHT].first = std::max(moves[RIGHT].first, table[x + 1][y + 1]);
      }
    }

    sort(moves.rbegin(), moves.rend());
    return moves;
  }
}