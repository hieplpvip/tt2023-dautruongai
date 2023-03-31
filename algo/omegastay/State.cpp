#include "State.h"
#include "Store.h"

bool State::isTerminal() const {
  // TODO: cache this value

  if (playerToMove != 0) {
    return false;
  }

  if (turnLeft == 0 || (!alive[0] && !alive[1])) {
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

void State::performMove(MoveEnum move) {
  lastMove[playerToMove] = move;

  if (playerToMove == 1) {
    auto [x, y] = pos[playerToMove];
    x += dx[move];
    y += dy[move];
    pos[playerToMove] = {x, y};

    --turnLeft;
  }

  playerToMove = 1 - playerToMove;
}
