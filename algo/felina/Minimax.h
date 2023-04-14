#ifndef MINIMAX_H
#define MINIMAX_H

#include "State.h"
#include <utility>
#include <vector>

namespace Minimax {
  std::pair<score_t, Position> MaxStartNode(score_t alpha, score_t beta, int depth, std::vector<std::pair<score_t, Position>> &candidates);
  std::pair<score_t, Position> MinStartNode(score_t alpha, score_t beta, int depth, std::vector<std::pair<score_t, Position>> &candidates);
  std::pair<score_t, Position> MaxNode(score_t alpha, score_t beta, int depth, State &state);
  std::pair<score_t, Position> MinNode(score_t alpha, score_t beta, int depth, State &state);
  void run();
}

#endif
