#include "Constants.h"
#include "Engine.h"
#include "Heuristic.h"
#include "MCTS.h"
#include "Negamax.h"
#include "Random.h"
#include "State.h"
#include "Store.h"
#include "Utility.h"
#include <cassert>
#include <iostream>
#include <vector>

namespace Engine {
  void findStartingPosition() {
    // Get symmetric positions
    static std::vector<Position> sym[20][20];
    {
      bool horizontal = true;
      bool vertical = true;
      bool main_diagonal = (Store::M == Store::N);
      bool anti_diagonal = (Store::M == Store::N);

      REPL_ALL_CELL(i, j) {
        if (rootState.at[i][j] != rootState.at[Store::M - i - 1][j]) {
          horizontal = false;
        }
        if (rootState.at[i][j] != rootState.at[i][Store::N - j - 1]) {
          vertical = false;
        }

        if (Store::M == Store::N) {
          if (rootState.at[i][j] != rootState.at[j][i]) {
            main_diagonal = false;
          }
          if (rootState.at[i][j] != rootState.at[Store::M - i - 1][Store::N - j - 1]) {
            anti_diagonal = false;
          }
        }
      }

      REPL_ALL_CELL(i, j) {
        sym[i][j].clear();
        sym[i][j].emplace_back(i, j);
        if (horizontal) {
          sym[i][j].emplace_back(Store::M - i - 1, j);
        }
        if (vertical) {
          sym[i][j].emplace_back(i, Store::N - j - 1);
        }
        if (main_diagonal) {
          sym[i][j].emplace_back(j, i);
        }
        if (anti_diagonal) {
          sym[i][j].emplace_back(Store::M - i - 1, Store::N - j - 1);
        }
      }
    }

    // Find top 10 candidates using heuristics
    auto candidates = Heuristic::GetCandidates(rootState, 10);
    int k = candidates.size();

    // Initialize MCTS tree
    // First two levels need to be handled specially
    MCTS::Node* root = MCTS::newStartNode(rootState);
    std::vector<std::pair<MCTS::Node*, std::vector<MCTS::Node*>>> child;
    for (int i = 0; i < k; ++i) {
      auto [x1, y1] = candidates[i].second;
      MCTS::Node* firstPlayerNode = MCTS::newStartNode(rootState, root, {x1, y1});
      child.push_back({firstPlayerNode, {}});

      for (int j = 0; j < k; ++j) {
        if (j != i) {
          auto [x2, y2] = candidates[j].second;
          MCTS::Node* secondPlayerNode = MCTS::newStartNode(firstPlayerNode->gameState, firstPlayerNode, {x2, y2});
          child.back().second.push_back(secondPlayerNode);

          // Warmup
          for (int t = 0; t < MCTS_MIN_VISITS; ++t) {
            MCTS::search(secondPlayerNode);
          }
        }
      }
    }

    // Find best start position using Monte Carlo tree search
    int countIterations = 0;
    Position lastPos = {-1, -1};
    while (countIterations < MCTS_MAX_ITERATIONS) {
      ++countIterations;

      int numSimulations = MCTS_NUM_SIMULATIONS;
      while (numSimulations--) {
        // Perform selection phase for first two levels
        int bestFirstIdx = -1;
        MCTS::Node* bestSecondNode = nullptr;
        {
          double bestScore = -INF;
          for (int i = 0; i < k; ++i) {
            double score = child[i].first->getUCT();
            if (score > bestScore) {
              bestScore = score;
              bestFirstIdx = i;
            }
          }
          assert(bestFirstIdx != -1);
        }
        {
          double bestScore = -INF;
          for (auto cand : child[bestFirstIdx].second) {
            double score = cand->getUCT();
            if (score > bestScore) {
              bestScore = score;
              bestSecondNode = cand;
            }
          }
          assert(bestSecondNode != nullptr);
        }

        // Run MCTS from the selected node
        MCTS::search(bestSecondNode);
      }

      // Find the most chosen position
      Position bestPos = {-1, -1};
      int maxNumVisits = 0;
      for (int i = 0; i < k; ++i) {
        auto node = child[i].first;
        if (node->numVisits > maxNumVisits) {
          maxNumVisits = node->numVisits;
          bestPos = node->gameState.pos[0];
        }
      }

      if (bestPos != lastPos) {
        lastPos = bestPos;

        // Randomly choose among the symmetric positions
        auto& A = sym[bestPos.x][bestPos.y];
        bestPos = A[Random::rand(A.size())];
        printFinalMove(bestPos.x, bestPos.y);

#ifdef ENABLE_LOGGING
        std::cerr << "Found new best position " << bestPos.x + 1 << ' ' << bestPos.y + 1 << ' ' << countIterations << std::endl;
#endif
      }
    }

#ifdef ENABLE_LOGGING
    for (int i = 0; i < k; ++i) {
      auto [x, y] = candidates[i].second;
      auto node = child[i].first;
      std::cerr << "Position " << x + 1 << ' ' << y + 1 << ": " << node->numVisits << " visits, win rate: " << (1 - node->winRate) << std::endl;
    }
#endif
  }

  void findNextMove() {
    if (rootState.turnLeft <= NEGAMAX_MAX_TURN_LEFT) {
      // Use Negamax for optimal play
      auto [_score, pos] = Negamax::negamax(rootState, -INF, INF, rootState.turnLeft > 8);

#ifdef ENABLE_LOGGING
      std::cerr << "Negamax score: " << _score << std::endl;
#endif

      printFinalMove(pos.x, pos.y);
      return;
    }

#ifdef TAKE_SHIELD_IMMEDIATELY
    if (!rootState.hasShield[0]) {
      auto [x, y] = rootState.pos[0];
      for (int k = 0; k < 4; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];
        if (isValidPos(nx, ny) && rootState.at[nx][ny] == SHIELD_CELL) {
#ifdef ENABLE_LOGGING
          std::cerr << "Take shield " << nx + 1 << ' ' << ny + 1 << std::endl;
#endif
          if (rootState.gold[0] > rootState.gold[1] || !rootState.pos[1].isAdjacent({nx, ny})) {
            // Take shield when we are having more gold or opponent is not standing next to shield
            printFinalMove(nx, ny);
            return;
          }
        }
      }
    }
#endif

#ifdef TAKE_GOLD_IMMEDIATELY
    {
      // Find adjacent gold cells
      int gold = -1, best_x = -1, best_y = -1;
      auto [x, y] = rootState.pos[0];
      for (int k = 0; k < 4; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];
        if (isValidPos(nx, ny) && rootState.at[nx][ny] != SHIELD_CELL && rootState.at[nx][ny] != DANGER_CELL && rootState.at[nx][ny] > gold) {
          gold = rootState.at[nx][ny];
          best_x = nx;
          best_y = ny;
        }
      }

      if (Store::gamePhase == LATE_GAME && gold >= 3) {
#ifdef ENABLE_LOGGING
        std::cerr << "Take " << gold << " gold " << best_x + 1 << ' ' << best_y + 1 << std::endl;
#endif
        printFinalMove(best_x, best_y);
        return;
      }
    }
#endif

    // Find best move using Monte Carlo tree search
    MCTS::Node* root = MCTS::newNode(rootState);
    int countIterations = 0;
    int lastMove = -1;
    while (countIterations < MCTS_MAX_ITERATIONS) {
      ++countIterations;

      auto move = MCTS::findBestMove(root, MCTS_NUM_SIMULATIONS);
      if (move != lastMove) {
        lastMove = move;
        int x = rootState.pos[0].x + dx[move];
        int y = rootState.pos[0].y + dy[move];
        printFinalMove(x, y);

#ifdef ENABLE_LOGGING
        std::cerr << "Found new best move " << x + 1 << ' ' << y + 1 << ' ' << countIterations << std::endl;
#endif
      }
    }

#ifdef ENABLE_LOGGING
    MCTS::printStats(root);
#endif
  }
}
