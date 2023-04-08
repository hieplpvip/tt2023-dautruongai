#ifndef MINIMAX_H
#define MINIMAX_H

#include "State.h"
#include <utility>

namespace MiniMax {
  std::pair<score_t, Position> MaxStartNode(score_t alpha, score_t beta, int depth);
  std::pair<score_t, Position> MinStartNode(score_t alpha, score_t beta, int depth);
  std::pair<score_t, Position> MaxNode(score_t alpha, score_t beta, int depth, State &state);
  std::pair<score_t, Position> MinNode(score_t alpha, score_t beta, int depth, State &state);
}

#endif
