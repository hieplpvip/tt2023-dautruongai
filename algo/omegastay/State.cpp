#include "State.h"
#include "Store.h"

bool State::isTerminal() const {
  // TODO: cache this value

  if (playerToMove != 0) {
    return false;
  }

  if (turnLeft == 0 || (eliminated[0] && eliminated[1])) {
    return true;
  }

  if (turnLeft > Store::K / 2 + 1) {
    return false;
  }

  if (gold[0] - gold[1] > 20 || gold[1] - gold[0] > 20) {
    return true;
  }

  return false;
}

int State::getResult() const {
  if (gold[0] > gold[1]) {
    return 1;
  } else if (gold[0] < gold[1]) {
    return -1;
  } else {
    return 0;
  }
}

void State::getLegalMoves(bool *isLegalMove, int &numLegalMoves) const {
  memset(isLegalMove, 0, NUM_MOVES * sizeof(bool));
  numLegalMoves = 0;

  if (eliminated[playerToMove]) {
    isLegalMove[0] = true;
    numLegalMoves = 1;
    return;
  }

  int x = pos[playerToMove].x;
  int y = pos[playerToMove].y;
  bool shield = hasShield[playerToMove];
  for (int k = 0; k < NUM_MOVES; ++k) {
    int nx = x + dx[k];
    int ny = y + dy[k];
    if (isValidPos(nx, ny) && (at[nx][ny] != DANGER_CELL || shield)) {
      isLegalMove[k] = true;
      ++numLegalMoves;
    }
  }
}

void State::performMove(MoveEnum move) {
  lastPos[playerToMove] = pos[playerToMove];
  if (!eliminated[playerToMove]) {
    auto &[x, y] = pos[playerToMove];
    x += dx[move];
    y += dy[move];
  }

  if (playerToMove == 1) {
    // Perform checks
    if (pos[0] == pos[1]) {
      // Move to same cell. Eliminate both.
      eliminated[0] = eliminated[1] = true;
    } else {
      // getLegalMoves guarantees ship will not move to danger cell
      // without shield, so no need to check that
      for (int i = 0; i < 2; ++i) {
        auto [x, y] = pos[i];
        if (at[x][y] == SHIELD_CELL) {
          hasShield[i] = true;
        } else if (at[x][y] != DANGER_CELL && at[x][y] != EMPTY_CELL) {
          gold[i] += at[x][y];
        }
      }
    }

    --turnLeft;

    // Treasure appears after half of the game has passed
    if (turnLeft == Store::K / 2) {
      int treasureValue = abs(gold[0] - gold[1]) * 3 / 4;
      at[Store::M / 2][Store::N / 2] = treasureValue;
    }
  }

  playerToMove = 1 - playerToMove;
}
