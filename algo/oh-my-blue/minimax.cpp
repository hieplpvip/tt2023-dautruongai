#include "minimax.h"
#include "heuristic.h"
#include "store.h"

#include <iostream>

#define isLegalMove(player, move) (Store::isLegalMove[state.hasShield[player]][state.pos[player].x][state.pos[player].y][move])

std::pair<score_t, Position> MiniMaxAlgorithm::MaxStartNode(score_t alpha, score_t beta, int depth, State &state) {
  score_t bestScore = 2 * -INF, secondBestScore = 2 * -INF;
  Position bestMove, secondBestMove;

  auto candidates = Heuristic::GetCandidates(rootState);

  for (auto &[_score, pos] : candidates) {
    rootState.pos[0] = rootState.lastPos[0] = pos;

    auto [score, _] = MinStartNode(alpha, beta, depth + 1, rootState);

    // Update the best score
    if (score > bestScore) {
      bestScore = score;
      bestMove = pos;
    } else if (score > secondBestScore) {
      secondBestScore = score;
      secondBestMove = pos;
    }

    // Alpha-beta pruning
    if (bestScore >= beta) {
      break;
    }
    if (bestScore > alpha) {
      alpha = bestScore;
    }
  }

  // FIXME: Use random(2) if needed

  return std::make_pair(bestScore, bestMove);
}

std::pair<score_t, Position> MiniMaxAlgorithm::MinStartNode(score_t alpha, score_t beta, int depth, State &state) {
  score_t bestScore = 2 * INF;
  Position bestMove;

  auto candidates = Heuristic::GetCandidates(rootState);

  for (auto &[_score, pos] : candidates) {
    rootState.pos[1] = rootState.lastPos[1] = pos;
    if (rootState.pos[0] == rootState.pos[1]) {
      continue;
    }

    auto [score, _] = MaxNode(alpha, beta, depth + 1, rootState);

    // Update the best score
    if (score < bestScore) {
      bestScore = score;
      bestMove = pos;
    }

    // Alpha-beta pruning
    if (bestScore <= alpha) {
      break;
    }
    if (bestScore < beta) {
      beta = bestScore;
    }
  }

  return std::make_pair(bestScore, bestMove);
}

std::pair<score_t, Position> MiniMaxAlgorithm::MaxNode(score_t alpha, score_t beta, int depth, State &state) {
  // Check terminal state
  if (state.isTerminal()) {
    return std::make_pair(state.getResult(), state.pos[0]);
  }

  // Check max depth
  if (depth >= MAX_DEPTH) {
    return std::make_pair(state.getScore(), state.pos[0]);
  }

  std::vector<MoveEnum> moves(4);
  for (int i = 0; i < NUM_MOVES; ++i) {
    moves[i] = static_cast<MoveEnum>(i);
  }

  // Remove invalid moves
  for (int i = 0; i < moves.size(); ++i) {
    if (!isLegalMove(PlayerEnum::ME, moves[i])) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // Avoid going back
  for (int i = 0; i < moves.size(); ++i) {
    int x = state.pos[PlayerEnum::ME].x + dx[moves[i]];
    int y = state.pos[PlayerEnum::ME].y + dy[moves[i]];
    if (Position{x, y} == state.lastPos[PlayerEnum::ME]) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // Try each move
  score_t bestScore = 2 * -INF;
  Position bestPos = state.pos[PlayerEnum::ME];
  for (auto move : moves) {
    // Perform the move
    State nextState = state;
    nextState.performMove(PlayerEnum::ME, move);

    // Get the score of the move
    auto [score, pos] = MinNode(alpha, beta, depth + 1, nextState);

    // Update the best score
    if (score > bestScore) {
      bestScore = score;
      bestPos = nextState.pos[PlayerEnum::ME];
    }

    // Alpha-beta pruning
    if (bestScore >= beta) {
      break;
    }
    if (bestScore > alpha) {
      alpha = bestScore;
    }
  }

  return std::make_pair(bestScore, bestPos);
}

std::pair<score_t, Position> MiniMaxAlgorithm::MinNode(score_t alpha, score_t beta, int depth, State &state) {
  std::vector<MoveEnum> moves(4);
  for (int i = 0; i < NUM_MOVES; ++i) {
    moves[i] = static_cast<MoveEnum>(i);
  }

  // Remove invalid moves
  for (int i = 0; i < moves.size(); ++i) {
    if (!isLegalMove(PlayerEnum::ENEMY, moves[i])) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // Avoid going back
  for (int i = 0; i < moves.size(); ++i) {
    int x = state.pos[PlayerEnum::ENEMY].x + dx[moves[i]];
    int y = state.pos[PlayerEnum::ENEMY].y + dy[moves[i]];
    if (Position{x, y} == state.lastPos[PlayerEnum::ENEMY]) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // Try each move
  score_t bestScore = 2 * INF;
  Position bestPos = state.pos[PlayerEnum::ENEMY];
  for (auto move : moves) {
    // Perform the move
    State nextState = state;
    nextState.performMove(PlayerEnum::ENEMY, move);

    // Get the score of the move
    auto [score, pos] = MaxNode(alpha, beta, depth + 1, nextState);

    // Update the best score
    if (score < bestScore) {
      bestScore = score;
      bestPos = nextState.pos[PlayerEnum::ENEMY];
    }

    // Alpha-beta pruning
    if (bestScore <= alpha) {
      break;
    }
    if (bestScore < beta) {
      beta = bestScore;
    }
  }

  return std::make_pair(bestScore, bestPos);
}
