#include "Heuristic.h"
#include "Store.h"
#include <algorithm>
#include <cmath>
#include <cstring>

#define SHIELD_VALUE 100
#define sqr(a) ((a) * (a))
#define f(gold, dist) (sqr(gold) / sqrt(dist))

using std::min, std::max;

// TODO: add comments to this fucking piece of code

namespace Heuristic {
  std::tuple<double, int, int> HighestHeat(State& state, int range, int player) {
    // Index from 1 for easier calculation
    static double score[20][20];

    // Reset score to zero
    memset(score, 0, sizeof(score));

    if (state.turnLeft > Store::HALF_K) {
      // Still in the first half of the game
      // Approximate treasure value
      state.at[Store::M / 2][Store::N / 2] = min(100, sqr(abs(state.gold[0] - state.gold[1]) * 3 / 4) / (state.turnLeft - Store::HALF_K + 2));
    }

    int opponent = player ^ 1;
    bool hasShield = state.hasShield[player];
    bool opponentHasShield = state.hasShield[opponent];
    auto ourDist = Store::dist[hasShield][state.pos[player].x][state.pos[player].y];
    auto opponentDist = Store::dist[opponentHasShield][state.pos[opponent].x][state.pos[opponent].y];

    for (int x = 0; x < Store::M; ++x) {
      for (int y = 0; y < Store::N; ++y) {
        if (ourDist[x][y] != INF) {
          double val = 0;

          if (state.at[x][y] == SHIELD_CELL) {
            // Ignore shield if already has shield
            val = hasShield ? 0 : SHIELD_VALUE;
          } else if (state.at[x][y] != DANGER_CELL) {
            val = state.at[x][y];
          }

          // Add bonus for each adjacent gold
          for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (isValidPos(nx, ny)) {
              if (state.at[nx][ny] == SHIELD_CELL) {
                if (!hasShield) {
                  val += 0.7 * SHIELD_VALUE;
                }
              } else if (state.at[nx][ny] != DANGER_CELL) {
                val += 0.7 * state.at[nx][ny];
              }
            }
          }

          if (opponentDist[x][y] <= ourDist[x][y]) {
            val = val * 2 / 3;
          }

          val = f(val + 1, ourDist[x][y] + 1);

          // Note that x and y are 0-indexed, but score is 1-indexed
          // Add val to all cells between (i1, j1) and (i2, j2)
          int i1 = max(1, x + 1 - range), j1 = max(1, y + 1 - range);
          int i2 = min(Store::M, x + 1 + range), j2 = min(Store::N, y + 1 + range);
          score[i1][j1] += val;
          score[i1][j2 + 1] -= val;
          score[i2 + 1][j1] -= val;
          score[i2 + 1][j2 + 1] += val;
        }
      }
    }

    if (state.turnLeft > Store::HALF_K) {
      // Reset center to empty
      state.at[Store::M / 2][Store::N / 2] = EMPTY_CELL;
    }

    double bestScore = -INF;
    int bestX = 1, bestY = 1;
    for (int i = 1; i <= Store::M; ++i) {
      for (int j = 1; j <= Store::N; ++j) {
        score[i][j] += score[i - 1][j] + score[i][j - 1] - score[i - 1][j - 1];

        if ((hasShield || state.at[i - 1][j - 1] != DANGER_CELL) && score[i][j] > bestScore) {
          bestScore = score[i][j];
          bestX = i;
          bestY = j;
        }
      }
    }

    return {bestScore, bestX - 1, bestY - 1};
  }

  double evaluate(const State& state, int shield, int x, int y) {
    auto dist = Store::dist[shield][x][y];
    double value = 0;
    for (int i = 0; i < Store::M; ++i) {
      for (int j = 0; j <= Store::N; ++j) {
        if (state.at[i][j] != EMPTY_CELL && state.at[i][j] != DANGER_CELL && state.at[i][j] != SHIELD_CELL) {
          value += (double)state.at[i][j] * pow(0.65, dist[i][j]);
        }
      }
    }
    if (state.at[x][y] != EMPTY_CELL && state.at[x][y] != DANGER_CELL && state.at[x][y] != SHIELD_CELL) {
      value += state.at[x][y];
    }
    return value / 2;
  }
}
