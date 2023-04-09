#ifndef ENGINE_H
#define ENGINE_H

#include "State.h"
#include "Utility.h"
#include <vector>

extern std::vector<Position> shieldPos;

namespace MCTSEngine {
  // Find starting position (turn 1)
  void findStartingPosition();

  // Find next move (turn 2 and later)
  void findNextMove();
}

#endif
