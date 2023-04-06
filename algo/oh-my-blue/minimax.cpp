#include "minimax.h"
#include "heuristic.h"
#include "store.h"

#include <iostream>

#define isLegalMove(player, move) (Store::isLegalMove[state.hasShield[player]][state.pos[player].x][state.pos[player].y][move])

std::pair<score_t, Position> MiniMaxAlgorithm::MaxNode(score_t alpha, score_t beta, int depth, State &state) {
  // Check terminal state
  if (state.isTerminal()) {
    return std::make_pair(state.getResult(), state.pos[0]);
  }

  // Get the best moves for me
  auto moves = Heuristic::GetBestMoves(state, PlayerEnum::ME);

  // Remove invalid moves
  for (int i = 0; i < moves.size(); ++i) {
    if (!isLegalMove(PlayerEnum::ME, moves[i].second)) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // Avoid going back
  for (int i = 0; i < moves.size(); ++i) {
    int x = state.pos[PlayerEnum::ME].x + dx[moves[i].second];
    int y = state.pos[PlayerEnum::ME].y + dy[moves[i].second];
    if (Position{x, y} == state.lastPos[PlayerEnum::ME]) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // If reach max depth, use magnet
  int branchingFactor = depth >= MAX_DEPTH ? 1 : BRANCHING_FACTOR;
  branchingFactor = std::min(branchingFactor, (int)moves.size());

  // Try each move
  score_t bestScore = -INF;
  Position bestPos = state.pos[PlayerEnum::ME];
  for (int i = 0; i < branchingFactor; ++i) {
    auto [_, move] = moves[i];

    // Perform the move
    State nextState = state;
    if (depth >= MAX_DEPTH) {
      nextState.performMoveWithMagnet(PlayerEnum::ME, move);
    } else {
      nextState.performMove(PlayerEnum::ME, move);
    }

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
  // Get the best moves for me
  auto moves = Heuristic::GetBestMoves(state, PlayerEnum::ENEMY);

  // Remove invalid moves
  for (int i = 0; i < moves.size(); ++i) {
    if (!isLegalMove(PlayerEnum::ENEMY, moves[i].second)) {
      std::swap(moves[i], moves.back());
      moves.pop_back();
      --i;
    }
  }

  // If reach max depth, use magnet
  int branchingFactor = depth >= MAX_DEPTH ? 1 : BRANCHING_FACTOR;
  branchingFactor = std::min(branchingFactor, (int)moves.size());

  // Try each move
  score_t bestScore = INF;
  Position bestPos = state.pos[PlayerEnum::ENEMY];
  for (int i = 0; i < branchingFactor; ++i) {
    auto [_, move] = moves[i];

    // Perform the move
    State nextState = state;
    if (depth >= MAX_DEPTH) {
      nextState.performMoveWithMagnet(PlayerEnum::ENEMY, move);
    } else {
      nextState.performMove(PlayerEnum::ENEMY, move);
    }

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