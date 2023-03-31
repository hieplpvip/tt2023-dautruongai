#include "MCTS.h"
#include "Store.h"
#include "Utility.h"
#include <cassert>
#include <cmath>
#include <vector>

using Node = MonteCarloTreeSearch::Node;

Node::Node(const State& gameState) : gameState(gameState) {
  // Populate legal moves
  int x = gameState.pos[gameState.playerToMove].x;
  int y = gameState.pos[gameState.playerToMove].y;
  bool hasShield = gameState.hasShield[gameState.playerToMove];
  for (int k = 0; k < NUM_MOVES; ++k) {
    int nx = x + dx[k];
    int ny = y + dy[k];
    if (isValidPos(nx, ny) && (gameState.at[nx][ny] != DANGER_CELL || hasShield)) {
      legalMoves[k] = true;
      ++numLegalMoves;
    }
  }
}

Node::Node(const State& gameState, Node* parent) : Node(gameState) {
  this->parent = parent;
}

bool Node::isFullyExpanded() const {
  return numChildren == numLegalMoves;
}

double Node::getUCT() const {
  assert(numVisits > 0);
  double parentVisits = 0.0;
  if (parent != nullptr) {
    parentVisits = parent->numVisits;
  }
  double uct = (double)sumScore / (double)numVisits;
  // if (player != player_to_move) {
  //   ratio = (visits - score) / visits;
  // }
  uct += MCTS_C * sqrt(log(parentVisits) / numVisits);
  return uct;
}

Node* Node::getBestChild() const {
  Node* bestChild = nullptr;
  double bestScore = -1e9;
  for (int k = 0; k < NUM_MOVES; ++k) {
    if (!legalMoves[k] || !children[k]) {
      continue;
    }
    double score = children[k]->getUCT();
    if (score > bestScore) {
      bestScore = score;
      bestChild = children[k];
    }
  }
  assert(bestChild != nullptr);  // TODO: remove this
  return bestChild;
}

MonteCarloTreeSearch::MonteCarloTreeSearch(const State& rootState) {
  root = new Node(rootState);
}

MoveEnum MonteCarloTreeSearch::findBestMove(int numIterations) {
  // Perform numIterations iterations of MCTS
  while (numIterations--) {
    search();
  }

  // Find the most visited child and return the corresponding move
  int bestMove = -1;
  int mostVisited = 0;
  for (int k = 0; k < NUM_MOVES; ++k) {
    if (!root->children[k]) {
      continue;
    }
    if (root->children[k]->numVisits > mostVisited) {
      mostVisited = root->children[k]->numVisits;
      bestMove = k;
    }
  }
  assert(bestMove != -1);  // TODO: remove this
  return static_cast<MoveEnum>(bestMove);
}

void MonteCarloTreeSearch::search() {
  Node* cur = root;

  // Selection phase
  // Traverse the tree from root, selecting the best child at each step
  // until we reach a non fully-expanded node
  while (cur->isFullyExpanded()) {
    cur = cur->getBestChild();
  }

  // Expansion phase
  // If the node is not a terminal node (hence non fully-expanded),
  // create a new child node for an untried move
  if (!cur->gameState.isTerminal()) {
    bool found = false;
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!cur->legalMoves[k] || cur->children[k]) {
        continue;
      }
      found = true;

      Node* child = new Node(cur->gameState, cur);
      assert(child != nullptr);
      child->gameState.performMove(static_cast<MoveEnum>(k));
      cur->children[k] = child;
      cur = child;

      break;
    }
    assert(found);  // TODO: remove this
  }

  // Simulation phase
  // Play out the game randomly from the new node until the end
  State tmpState = cur->gameState;
  while (!tmpState.isTerminal()) {
    auto move = getRandomMove(tmpState);
    tmpState.performMove(move);
  }

  // Backpropagation phase
  // Update the scores of all the nodes in the path from cur to root
  int result = tmpState.getResult();
  if (cur->gameState.playerToMove == 1) {
    result = -result;
  }
  do {
    cur->numVisits++;
    cur->sumScore += result;
    cur = cur->parent;
    result = 1 - result;
  } while (cur != nullptr);
}

MoveEnum MonteCarloTreeSearch::getRandomMove(const State& state) const {
  // TODO: use better simulation strategy
  int x = state.pos[state.playerToMove].x;
  int y = state.pos[state.playerToMove].y;
  bool hasShield = state.hasShield[state.playerToMove];
  std::vector<MoveEnum> legalMoves;
  for (int k = 0; k < NUM_MOVES; ++k) {
    int nx = x + dx[k];
    int ny = y + dy[k];
    if (isValidPos(nx, ny) && (state.at[nx][ny] != DANGER_CELL || hasShield)) {
      legalMoves.push_back(static_cast<MoveEnum>(k));
    }
  }
  assert(!legalMoves.empty());  // TODO: remove this
  return legalMoves[Random::rand(legalMoves.size())];
}
