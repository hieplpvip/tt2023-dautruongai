#ifndef NEGAMAX_H
#define NEGAMAX_H

#include "State.h"
#include <utility>

namespace Negamax {
  std::pair<score_t, Position> negamax(State& state, int alpha, int beta, bool avoidGoingBack);
}

#endif
