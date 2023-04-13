#include "Negamax.h"
#include <algorithm>

namespace Negamax {
  std::pair<score_t, Position> negamax(State& state, int alpha, int beta, bool avoidGoingBack) {
    if (state.isTerminal()) {
      return {state.getScore(), {-1, -1}};
    }

    int numLegalMoves;
    bool isLegalMove[NUM_MOVES];
    state.getLegalMoves(isLegalMove, numLegalMoves);

    // Avoid going back if having more than 3 legal moves
    if (avoidGoingBack && numLegalMoves > 3) {
      auto [x, y] = state.pos[state.playerToMove];
      auto [lastX, lastY] = state.lastPos[state.playerToMove];
      for (int k = 0; k < NUM_MOVES; ++k) {
        if (isLegalMove[k] && x + dx[k] == lastX && y + dy[k] == lastY) {
          isLegalMove[k] = false;
          --numLegalMoves;
          break;
        }
      }
    }

    int bestScore = -INF;
    Position bestPos = {-1, -1};
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!isLegalMove[k]) {
        continue;
      }

      State childState = state;
      childState.performMove(static_cast<MoveEnum>(k));

      auto [score, pos] = negamax(childState, -beta, -alpha, avoidGoingBack);
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
