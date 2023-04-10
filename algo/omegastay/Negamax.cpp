#include "Negamax.h"
#include <algorithm>

namespace NegaMax {
  std::pair<score_t, Position> negamax(State& state, int alpha, int beta) {
    if (state.isTerminal()) {
      return {state.getScore(), {-1, -1}};
    }

    int numLegalMoves;
    bool isLegalMove[NUM_MOVES];
    state.getLegalMoves(isLegalMove, numLegalMoves);

    int bestScore = -INF;
    Position bestPos = {-1, -1};
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!isLegalMove[k]) {
        continue;
      }

      State childState = state;
      childState.performMove(static_cast<MoveEnum>(k));

      auto [score, pos] = negamax(childState, -beta, -alpha);
      score = -score;

      if (score > bestScore) {
        bestScore = score;
        bestPos = childState.pos[state.playerToMove];
      }

      alpha = std::max(alpha, bestScore);
      if (alpha >= beta) {
        break;
      }
    }

    return {bestScore, bestPos};
  }
}
