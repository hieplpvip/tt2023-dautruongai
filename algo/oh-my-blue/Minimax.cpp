#include "Minimax.h"
#include "Heuristic.h"
#include "Random.h"
#include "Store.h"
#include <iostream>
#include <algorithm>

#define isLegalMove(player, move) (Store::isLegalMove[state.hasShield[player]][state.pos[player].x][state.pos[player].y][move])
#define dist(a, b) (abs(a.x - b.x) + abs(a.y - b.y))

constexpr int BRANCHING_FACTOR = 2;
constexpr int BIAS = 5;

namespace MiniMax {
  std::pair<score_t, Position> MaxStartNode(score_t alpha, score_t beta, int depth, std::vector<std::pair<score_t, Position>> &candidates) {
    score_t bestScore = 2 * -INF, secondBestScore = 2 * -INF;
    Position bestMove = Position(0, 0);
    Position secondBestMove = bestMove;

    for (auto &[_score, pos] : candidates) {
      rootState.pos[0] = rootState.lastPos[0] = pos;

      auto [score, _] = MinStartNode(alpha, beta, depth + 1, candidates);

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

    // return Random::rand(2) ? std::make_pair(bestScore, bestMove) : std::make_pair(secondBestScore, secondBestMove);

    return std::make_pair(bestScore, bestMove);
  }

  std::pair<score_t, Position> MinStartNode(score_t alpha, score_t beta, int depth, std::vector<std::pair<score_t, Position>> &candidates) {
    score_t bestScore = 2 * INF;
    Position bestMove = Position(0, 0);

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

  std::pair<score_t, Position> MaxNode(score_t alpha, score_t beta, int depth, State &state) {
    // Check terminal state
    if (state.isTerminal()) {
      return std::make_pair(state.getResult(), state.pos[0]);
    }

    // Check max depth
    if (depth >= MAX_DEPTH) {
      return std::make_pair(state.getScore(), state.pos[0]);
    }

    std::vector<std::pair<score_t, MoveEnum>> moves;
    for (int i = 0; i < NUM_MOVES; ++i) {
      moves.emplace_back(0, static_cast<MoveEnum>(i));
    }

    // Remove invalid moves
    for (int i = 0; i < (int)moves.size(); ++i) {
      if (!isLegalMove(PlayerEnum::ME, moves[i].second)) {
        std::swap(moves[i], moves.back());
        moves.pop_back();
        --i;
      }
    }

    // Avoid going back
    for (int i = 0; i < (int)moves.size(); ++i) {
      int x = state.pos[PlayerEnum::ME].x + dx[moves[i].second];
      int y = state.pos[PlayerEnum::ME].y + dy[moves[i].second];
      if (Position{x, y} == state.lastPos[PlayerEnum::ME]) {
        std::swap(moves[i], moves.back());
        moves.pop_back();
        --i;
      }
    }

    // Sort direction by heat
    for (int i = 0; i < (int)moves.size(); ++i) {
      State nextState = state;
      nextState.performMove(PlayerEnum::ME, moves[i].second);
      moves[i].first = Heuristic::GetHighestHeat(nextState, PlayerEnum::ME);

      int cell = nextState.at[nextState.pos[PlayerEnum::ME].x][nextState.pos[PlayerEnum::ME].y];
      if (cell != DANGER_CELL) {
        moves[i].first += BIAS * cell;
      }
    }
    std::sort(moves.rbegin(), moves.rend());

    while (moves.size() > BRANCHING_FACTOR) {
      moves.pop_back();
    }

    // Try each move
    score_t bestScore = 2 * -INF;
    Position bestPos = state.pos[PlayerEnum::ME];
    for (auto [_, move] : moves) {
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

#ifdef ENABLE_LOGGING
      if (depth == 0) {
        std::cerr << "Move: " << move << " Score: " << score << std::endl;
      }
#endif
    }

    return std::make_pair(bestScore, bestPos);
  }

  std::pair<score_t, Position> MinNode(score_t alpha, score_t beta, int depth, State &state) {
    std::vector<std::pair<score_t, MoveEnum>> moves;
    for (int i = 0; i < NUM_MOVES; ++i) {
      moves.emplace_back(0, static_cast<MoveEnum>(i));
    }

    // Remove invalid moves
    for (int i = 0; i < (int)moves.size(); ++i) {
      if (!isLegalMove(PlayerEnum::ENEMY, moves[i].second)) {
        std::swap(moves[i], moves.back());
        moves.pop_back();
        --i;
      }
    }

    // Avoid going back
    for (int i = 0; i < (int)moves.size(); ++i) {
      int x = state.pos[PlayerEnum::ENEMY].x + dx[moves[i].second];
      int y = state.pos[PlayerEnum::ENEMY].y + dy[moves[i].second];
      if (Position{x, y} == state.lastPos[PlayerEnum::ENEMY]) {
        std::swap(moves[i], moves.back());
        moves.pop_back();
        --i;
      }
    }

    // Sort direction by heat
    for (int i = 0; i < (int)moves.size(); ++i) {
      State nextState = state;
      nextState.performMove(PlayerEnum::ENEMY, moves[i].second);
      moves[i].first = Heuristic::GetHighestHeat(nextState, PlayerEnum::ENEMY);

      int cell = nextState.at[nextState.pos[PlayerEnum::ENEMY].x][nextState.pos[PlayerEnum::ENEMY].y];
      if (cell != DANGER_CELL) {
        moves[i].first += BIAS * cell;
      }
    }

    while (moves.size() > BRANCHING_FACTOR) {
      moves.pop_back();
    }

    // Try each move
    score_t bestScore = 2 * INF;
    Position bestPos = state.pos[PlayerEnum::ENEMY];
    for (auto [_, move] : moves) {
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
}
