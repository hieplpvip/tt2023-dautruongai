#include "State.h"
#include "Store.h"
#include <cassert>
#include <cstring>
#include <iostream>

bool State::isTerminal() const {
  if (playerToMove != 0) {
    return false;
  }

  if (turnLeft == 0 || (eliminated[0] && eliminated[1])) {
    return true;
  }

  if (turnLeft > Store::HALF_K + 1) {
    return false;
  }

  if (gold[0] - gold[1] > 20 || gold[1] - gold[0] > 20) {
    return true;
  }

  return false;
}

int State::getResult() const {
#ifdef DRAW_IS_UNACCEPTABLE
  return (gold[0] > gold[1]) ? 1 : -1;
#else
  if (gold[0] > gold[1]) {
    return 1;
  } else if (gold[0] < gold[1]) {
    return -1;
  } else {
    return 0;
  }
#endif
}

const bool isLegalMoveWhenEliminated[NUM_MOVES] = {true, false, false, false};

void State::getLegalMoves(bool *isLegalMove, int &numLegalMoves) const {
  if (eliminated[playerToMove]) {
    numLegalMoves = 1;
    memcpy(isLegalMove, isLegalMoveWhenEliminated, NUM_MOVES * sizeof(bool));
    return;
  }

  int x = pos[playerToMove].x;
  int y = pos[playerToMove].y;
  if (hasShield[playerToMove]) {
    numLegalMoves = Store::numLegalMovesWithShield[x][y];
    memcpy(isLegalMove, Store::isLegalMoveWithShield[x][y], NUM_MOVES * sizeof(bool));
  } else {
    numLegalMoves = Store::numLegalMovesNoShield[x][y];
    memcpy(isLegalMove, Store::isLegalMoveNoShield[x][y], NUM_MOVES * sizeof(bool));
  }
}

void State::performMove(MoveEnum move) {
  lastPos[playerToMove] = pos[playerToMove];
  if (!eliminated[playerToMove]) {
    auto &[x, y] = pos[playerToMove];
    x += dx[move];
    y += dy[move];
    assert(isValidPos(x, y) && (at[x][y] != DANGER_CELL || hasShield[playerToMove]));
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
          at[x][y] = EMPTY_CELL;
        } else if (at[x][y] != DANGER_CELL && at[x][y] != EMPTY_CELL) {
          gold[i] += at[x][y];
          at[x][y] = EMPTY_CELL;
        }
      }
    }

    --turnLeft;

    // Treasure appears after half of the game has passed
    if (turnLeft == Store::HALF_K) {
      int treasureValue = abs(gold[0] - gold[1]) * 3 / 4;
      at[Store::M / 2][Store::N / 2] = treasureValue;
    }
  }

  playerToMove = 1 - playerToMove;
}

void State::printState() const {
#ifdef ENABLE_DEBUG_MODE
  using std::cout, std::endl;

  cout << turnLeft << " turns left" << endl;
  cout << (playerToMove ? "Second" : "First") << " player to move" << endl;
  for (int i = 0; i < 2; ++i) {
    cout << (i ? "Second" : "First") << " player: at (";
    cout << (pos[i].x + 1) << ", " << (pos[i].y + 1) << "), ";
    cout << (eliminated[i] ? "eliminated" : "alive") << ", ";
    cout << (hasShield[i] ? "has shield" : "no shield") << ", ";
    cout << gold[i] << " gold" << endl;
  }

  for (int i = 0; i < Store::M; ++i) {
    for (int j = 0; j < Store::N; ++j) {
      switch (at[i][j]) {
        case DANGER_CELL:
          cout << 'D';
          break;

        case SHIELD_CELL:
          cout << 'S';
          break;

        default:
          // Gold
          cout << (int)at[i][j];
          break;
      }
      cout << ' ';
    }
    cout << endl;
  }
#endif
}
