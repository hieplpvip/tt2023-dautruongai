#include "State.h"
#include "Store.h"
#include "Heuristic.h"
#include <algorithm>
#include <cassert>
#include <iostream>

#define sqr(a) ((a) * (a))
#define dist(player, x, y) (Store::dist[hasShield[player]][pos[player].x][pos[player].y][x][y])

constexpr int SHIELD_VALUE = 7;

bool State::isTerminal() const {
  if (turnLeft == 0 || (eliminated[0] && eliminated[1])) {
    return true;
  }

  if (turnLeft > Store::HALF_K) {
    return false;
  }

  if (gold[0] - gold[1] > 20 || gold[1] - gold[0] > 20) {
    return true;
  }

  return false;
}

score_t State::getResult() const {
  if (gold[0] > gold[1]) {
    return INF + score[0] - score[1];
  }
  if (gold[0] < gold[1]) {
    return -INF + score[0] - score[1];
  }
  return score[0] - score[1];
}

score_t State::getScore() {
  return (score[0] + Heuristic::GetHighestHeat(*this, PlayerEnum::ME)) - (score[1] + Heuristic::GetHighestHeat(*this, PlayerEnum::ENEMY));
}

void State::performMove(PlayerEnum player, MoveEnum move) {
  lastPos[player] = pos[player];
  if (!eliminated[player]) {
    auto &[x, y] = pos[player];
    x += dx[move];
    y += dy[move];
    assert(isValidPos(x, y) && (at[x][y] != DANGER_CELL || hasShield[player]));
  }

  if (player == PlayerEnum::ENEMY) {
    // Both players have moved. Update the game state.
    if (pos[0] == pos[1] || (pos[0] == lastPos[1] && pos[1] == lastPos[0])) {
      // Move to same cell or swap cells. Eliminate both.
      eliminated[0] = eliminated[1] = true;
    } else {
      // getLegalMoves guarantees ship will not move to danger cell
      // without shield, so no need to check that
      for (int i = 0; i < 2; ++i) {
        auto [x, y] = pos[i];
        if (at[x][y] == SHIELD_CELL) {
          if (i != PlayerEnum::ENEMY && !hasShield[i] && Store::gamePhase != GamePhaseEnum::LATE_GAME) {
            // score[i] += Heuristic::Evaluate(SHIELD_VALUE, Store::K - turnLeft - Store::currentTurn + 3);
            score[i] += sqr(SHIELD_VALUE + 1);
          }
          hasShield[i] = true;
          at[x][y] = EMPTY_CELL;
        } else if (at[x][y] != DANGER_CELL && at[x][y] != EMPTY_CELL) {
          gold[i] += at[x][y];
          // score[i] += Heuristic::Evaluate(at[x][y], Store::K - turnLeft - Store::currentTurn + 3);
          score[i] += sqr(at[x][y] + 1);
          at[x][y] = EMPTY_CELL;
        }
      }
    }

    --turnLeft;

    // Treasure appears after half of the game has passed
    if (turnLeft == Store::HALF_K) {
      int treasureValue = std::max(1, abs(gold[0] - gold[1]) * 3 / 4);
      at[Store::M / 2][Store::N / 2] = treasureValue;
    }
  }
}
