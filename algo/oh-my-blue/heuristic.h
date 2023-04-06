#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <vector>
#include "state.h"

#define sqr(a) ((a) * (a))

namespace Heuristic {
  /*
   * Get the best moves for a player.
   * The returned moves is sorted in descending order of score.
   */
  std::vector<std::pair<score_t, MoveEnum>> GetBestMoves(State state, PlayerEnum player);
};

#endif  // HEURISTIC_H