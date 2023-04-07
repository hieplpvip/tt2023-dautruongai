#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "State.h"
#include <array>
#include <utility>

namespace Heuristic {
  std::tuple<double, int, int> HighestHeat(State& state, int range, int player);

  double evaluate(const State& state, int shield, int x, int y);
};

#endif
