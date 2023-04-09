#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "State.h"
#include <utility>
#include <vector>

namespace Heuristic {
  /*
   * Get candidates for optimal first place.
   */
  std::vector<std::pair<score_t, Position>> GetCandidates(State& state);

  /*
   * Get the highest heat value corresponding to player.
   */
  score_t GetHighestHeat(State& state, PlayerEnum player);

  /*
   * Evaluate the score of a cell.
   */
  score_t Evaluate(double gold, int distance);
}

#endif
