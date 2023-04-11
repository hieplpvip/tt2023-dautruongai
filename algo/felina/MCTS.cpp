#include "MCTS.h"
#include "Random.h"
#include "Store.h"
#include "Utility.h"
#include <cassert>
#include <iostream>
#include <vector>

namespace MCTS {
  static int numNodes = 0;
  static std::vector<Node> nodes(NUMBER_OF_PREALLOCATED_NODES);

  // Used to make sure that first two levels of the tree are properly assessed
  constexpr int MIN_VISITS_BY_DEPTH[] = {12000, 3000};
  constexpr int SIZE_OF_MIN_VISITS_BY_DEPTH = sizeof(MIN_VISITS_BY_DEPTH) / sizeof(MIN_VISITS_BY_DEPTH[0]);

  Node* newRootNode() {
    if (numNodes == NUMBER_OF_PREALLOCATED_NODES) {
      std::cerr << "Out of preallocated nodes" << std::endl;
      exit(1);
    }

    auto& node = nodes[numNodes];
    node.gameState = rootState;
    node.gameState.getLegalMoves(&node.isLegalMove[0], node.numLegalMoves);
    node.isTerminal = node.gameState.isTerminal();
    return &nodes[numNodes++];
  }

  Node* newNode(const State& gameState, Node* parent, MoveEnum move, int depth) {
    if (numNodes == NUMBER_OF_PREALLOCATED_NODES) {
      std::cerr << "Out of preallocated nodes" << std::endl;
      exit(1);
    }

    auto& node = nodes[numNodes];
    node.parent = parent;
    node.gameState = gameState;
    node.gameState.performMove(move);
    node.gameState.getLegalMoves(&node.isLegalMove[0], node.numLegalMoves);
    node.isTerminal = node.gameState.isTerminal();

    // Avoid going back except for the first two levels of the tree
    // or when there is no other choice
    if (node.numLegalMoves > 1 && depth >= 2) {
      auto [x, y] = node.gameState.pos[node.gameState.playerToMove];
      auto [lastX, lastY] = node.gameState.lastPos[node.gameState.playerToMove];
      for (int k = 0; k < NUM_MOVES; ++k) {
        if (node.isLegalMove[k] && x + dx[k] == lastX && y + dy[k] == lastY) {
          node.isLegalMove[k] = false;
          --node.numLegalMoves;
          break;
        }
      }
    }

    return &nodes[numNodes++];
  }

  Node* newStartNode(const State& gameState) {
    if (numNodes == NUMBER_OF_PREALLOCATED_NODES) {
      std::cerr << "Out of preallocated nodes" << std::endl;
      exit(1);
    }

    auto& node = nodes[numNodes];
    node.gameState = gameState;
    return &nodes[numNodes++];
  }

  Node* newStartNode(const State& gameState, Node* parent, Position startPos) {
    if (numNodes == NUMBER_OF_PREALLOCATED_NODES) {
      std::cerr << "Out of preallocated nodes" << std::endl;
      exit(1);
    }

    auto& node = nodes[numNodes];
    node.parent = parent;
    node.gameState = gameState;
    node.gameState.pos[node.gameState.playerToMove] = startPos;
    node.gameState.playerToMove = static_cast<PlayerEnum>(1 - node.gameState.playerToMove);
    if (node.gameState.playerToMove == ME) {
      node.gameState.getLegalMoves(&node.isLegalMove[0], node.numLegalMoves);
      node.isTerminal = node.gameState.isTerminal();
    }
    return &nodes[numNodes++];
  }

  bool Node::isFullyExpanded() const {
    return numChildren == numLegalMoves;
  }

  double Node::getUCT() const {
    assert(numVisits > 0);
    // We never call this function on root, so parent is always non-null
    // UCT = (numVisits - sumResult) / numVisits + MCTS_C * sqrt(log(parent->numVisits) / numVisits)
    return 1.0 - winRate + CDivSqrtNumVisits * parent->sqrtLogNumVisits;
  }

  Node* Node::getBestChild(int depth) const {
    int minVisits = (depth < SIZE_OF_MIN_VISITS_BY_DEPTH) ? MIN_VISITS_BY_DEPTH[depth] : MCTS_MIN_VISITS;

    Node* bestChild = nullptr;
    double bestScore = -INF;
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!children[k]) {
        continue;
      }

      if (children[k]->numVisits < minVisits) {
        // If any child has not been visited often enough, return it
        return children[k];
      }

      double score = children[k]->getUCT();
      if (score > bestScore) {
        bestScore = score;
        bestChild = children[k];
      }
    }
    assert(bestChild != nullptr);
    return bestChild;
  }

  void printStats(const Node* root) {
    std::cerr << "Root: " << root->numVisits << " visits, win rate: " << root->winRate << std::endl;
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!root->children[k]) {
        continue;
      }
      auto child = root->children[k];
      std::cerr << "-------------------------------" << std::endl;
      std::cerr << "Child " << k << ": " << child->numVisits << " visits, win rate: " << child->winRate << std::endl;
      for (int h = 0; h < NUM_MOVES; ++h) {
        if (!child->children[h]) {
          continue;
        }
        auto grandChild = child->children[h];
        std::cerr << "Grandchild " << h << ": " << grandChild->numVisits << " visits, win rate: " << grandChild->winRate << std::endl;
      }
    }
  }

  MoveEnum findBestMove(Node* root, int numIterations) {
    // Perform numIterations iterations of MCTS
    while (numIterations--) {
      search(root);
    }

    // Find the most visited child and return the corresponding move
    int bestMove = -1;
    int maxNumVisits = 0;
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!root->children[k]) {
        continue;
      }
      if (root->children[k]->numVisits > maxNumVisits) {
        maxNumVisits = root->children[k]->numVisits;
        bestMove = k;
      }
    }
    assert(bestMove != -1);
    return static_cast<MoveEnum>(bestMove);
  }

  void search(Node* root) {
    Node* cur = root;

    // Selection phase
    // Traverse the tree from root, selecting the best child at each step
    // until we reach a non fully-expanded node
    int depth = 0;
    while (cur->isFullyExpanded()) {
      cur = cur->getBestChild(depth);
      ++depth;
    }

    // Expansion phase
    // If the node is not a terminal node (hence non fully-expanded),
    // create a new child node for an untried move
    if (!cur->isTerminal) {
      for (int k = 0; k < NUM_MOVES; ++k) {
        if (!cur->isLegalMove[k] || cur->children[k]) {
          continue;
        }

        Node* child = newNode(cur->gameState, cur, static_cast<MoveEnum>(k), depth);
        assert(child != nullptr);
        cur->children[k] = child;
        cur->numChildren++;
        cur = child;

        break;
      }
    }

    // Simulation phase
    // Play out the game randomly from the new node until the end
    int lastMove[2] = {-1, -1};
    for (int i = 0; i < 2; ++i) {
      auto [x, y] = cur->gameState.pos[i];
      auto [lastX, lastY] = cur->gameState.lastPos[i];
      for (int k = 0; k < 4; ++k) {
        if (x + dx[k] == lastX && y + dy[k] == lastY) {
          lastMove[i] = k;
          break;
        }
      }
    }
    State tmpState = cur->gameState;
    while (!tmpState.isTerminal()) {
      auto move = getRandomMove(tmpState, lastMove[tmpState.playerToMove]);
      lastMove[tmpState.playerToMove] = move;
      tmpState.performMove(move);
    }

    // Backpropagation phase
    // Update the scores of all the nodes in the path from cur to root
    int result = tmpState.getResult();
    if (cur->gameState.playerToMove == 1) {
      result = 1 - result;
    }
    do {
      cur->numVisits++;
      cur->sumScore += result;
      cur->winRate = (double)cur->sumScore / cur->numVisits;
      cur->sqrtLogNumVisits = sqrt(log(cur->numVisits));
      cur->CDivSqrtNumVisits = MCTS_C / sqrt(cur->numVisits);
      cur = cur->parent;
      result = 1 - result;
    } while (cur != nullptr);
  }

  MoveEnum getRandomMove(State& state, int lastMove) {
    static int prob[NUM_MOVES];
    static int numLegalMoves;
    static bool isLegalMove[NUM_MOVES];
    state.getLegalMoves(isLegalMove, numLegalMoves);

    auto [x, y] = state.pos[state.playerToMove];
    bool shield = state.hasShield[state.playerToMove];

    for (int k = 0; k < NUM_MOVES; ++k) {
      // Avoid going back unless there is no other choice
      if (isLegalMove[k] && ((k ^ 1) != lastMove || numLegalMoves == 1)) {
        prob[k] = 1;

        int nx = x + dx[k];
        int ny = y + dy[k];
        if (state.at[nx][ny] == SHIELD_CELL) {
          if (!shield) {
            prob[k] += 25;
          }
        } else if (state.at[nx][ny] != DANGER_CELL) {
          prob[k] += (state.at[nx][ny] << 3);
        }
      } else {
        prob[k] = 0;
      }
    }
    for (int k = 1; k < NUM_MOVES; ++k) {
      prob[k] += prob[k - 1];
    }
    assert(prob[NUM_MOVES - 1] > 0);

    int r = Random::rand(prob[NUM_MOVES - 1]);
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (r < prob[k]) {
        return static_cast<MoveEnum>(k);
      }
    }
    __builtin_unreachable();
  }
}
