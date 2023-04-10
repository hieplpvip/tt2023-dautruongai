#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "State.h"
#include <utility>
#include <vector>

namespace Heuristic {
  /*
   * Get candidates for optimal first place.
   */
  std::vector<std::pair<score_t, Position>> GetCandidates(State& state, int numCandidates);
}

#endif
