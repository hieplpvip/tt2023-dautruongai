#include "State.h"
#include "Store.h"
#include "Heuristic.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

#define sqr(a) ((a) * (a))

constexpr int SHIELD_VALUE = 7;

bool State::isTerminal() const {
  if (playerToMove != 0) {
    return false;
  }

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
  return (gold[0] > gold[1]) ? 1 : 0;
}

int State::getScore() const {
  return gold[0] - gold[1];
}

score_t State::getHeuristicResult() const {
  if (gold[0] > gold[1]) {
    return INF + hscore[0] - hscore[1];
  }
  if (gold[0] < gold[1]) {
    return -INF + hscore[0] - hscore[1];
  }
  return hscore[0] - hscore[1];
}

score_t State::getHeuristicScore() {
  return (hscore[0] + Heuristic::GetHighestHeat(*this, PlayerEnum::ME)) - (hscore[1] + Heuristic::GetHighestHeat(*this, PlayerEnum::ENEMY));
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
  int shield = hasShield[playerToMove];
  numLegalMoves = Store::numLegalMoves[shield][x][y];
  memcpy(isLegalMove, Store::isLegalMove[shield][x][y], NUM_MOVES * sizeof(bool));
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
          if (i == 0 && !hasShield[i] && Store::gamePhase != GamePhaseEnum::LATE_GAME) {
            hscore[i] += sqr(SHIELD_VALUE + 1);
          }
          hasShield[i] = true;
          at[x][y] = EMPTY_CELL;
        } else if (at[x][y] != DANGER_CELL && at[x][y] != EMPTY_CELL) {
          gold[i] += at[x][y];
          hscore[i] += sqr(at[x][y] + 1);
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

  playerToMove = static_cast<PlayerEnum>(1 - playerToMove);
}

void State::printState() const {
#ifdef ENABLE_LOGGING
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
