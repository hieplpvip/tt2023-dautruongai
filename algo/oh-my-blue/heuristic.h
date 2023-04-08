#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <vector>
#include "state.h"

#define sqr(a) ((a) * (a))

namespace Heuristic {
  /*
   * Generate heat map for a state according to player.
   * For each cell(x, y) it will compute the heat in the range of HEAT_RADIUS (currently 2).
   */
  std::vector<std::vector<score_t>> GetHeatMap(State state, PlayerEnum player);

  /*
   * Generate heat map for a state without player.
   * Use for initial state.
   */
  std::vector<std::vector<score_t>> GetHeatMap(State state);

  /*
   * Get candidates for optimal first place.
   */
  std::vector<std::pair<score_t, Position>> GetCandidates(State state);

  /*
   * Get the highest heat value corresponding to player.
   */
  score_t GetHighestHeat(State state, PlayerEnum player);
};

#endif  // HEURISTIC_H
