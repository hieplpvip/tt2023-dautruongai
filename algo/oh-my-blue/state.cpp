#include "state.h"
#include "store.h"

#include <assert.h>
#include <algorithm>
#include <iostream>

#define dist(player, x, y) (Store::dist[hasShield[player]][pos[player].x][pos[player].y][x][y])

State rootState;

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

int State::getResult() const {
  // FIXME: if it does not prioritize collecting gold, change to (BASE * gold - penalty)
  if (eliminated[0] && eliminated[1]) {
    if (gold[0] == gold[1]) {
      return 0;
    }
    return gold[0] > gold[1] ? INF : -INF;
  }
  return score[0] - score[1];
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
          if (!hasShield[i]) {
            score[i] += SHIELD_CELL - dist(i, x, y);  // make sure shield is always picked up
          }
          hasShield[i] = true;
          at[x][y] = EMPTY_CELL;
        } else if (at[x][y] != DANGER_CELL && at[x][y] != EMPTY_CELL) {
          gold[i] += at[x][y];
          score[i] += BASE_GOLD * at[x][y] - dist(i, x, y);
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

void State::performMoveWithMagnet(PlayerEnum player, MoveEnum move) {
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
        auto [a, b] = pos[i];
        REPL_MAGNET_CELL(x, y, a, b) {
          if (!isValidPos(x, y) || at[x][y] == DANGER_CELL) {
            continue;
          }

          if (at[x][y] == SHIELD_CELL) {
            if (!hasShield[i]) {
              score[i] += MAGNET_RATE * SHIELD_CELL - dist(i, x, y);  // make sure shield is always picked up
            }
            hasShield[i] = true;
            at[x][y] = EMPTY_CELL;
          } else if (at[x][y] != DANGER_CELL && at[x][y] != EMPTY_CELL) {
            score[i] += BASE_GOLD * MAGNET_RATE * at[x][y] - dist(i, x, y);
            at[x][y] = EMPTY_CELL;
          }
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