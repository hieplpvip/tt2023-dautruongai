#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "State.h"
#include <utility>
#include <vector>

namespace Heuristic {
  /*
   * Generate heat map for a state according to player.
   * For each cell(x, y) it will compute the heat in the range of HEAT_RADIUS (currently 2).
   */
  std::vector<std::vector<double>> GetHeatMap(State& state, int player);

  /*
   * Generate heat map for a state without player.
   * Use for initial state.
   */
  std::vector<std::vector<double>> GetHeatMap(State& state);

  /*
   * Get candidates for optimal first place.
   */
  std::vector<std::pair<double, Position>> GetCandidates(State& state);

  /*
   * Get the highest heat value corresponding to player.
   */
  double GetHighestHeat(State& state, int player);

  /*
   * Get the position with highest heat value corresponding to player.
   */
  Position GetHighestHeatPosition(State& state, int player);
};

#endif
