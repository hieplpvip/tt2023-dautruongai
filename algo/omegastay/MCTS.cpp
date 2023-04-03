#include "MCTS.h"
#include "Store.h"
#include "Utility.h"
#include <bits/stdc++.h>

using namespace std;
using Node = MonteCarloTreeSearch::Node;

Node::Node(const State& gameState) : gameState(gameState) {
  // Populate legal moves
  this->gameState.getLegalMoves(&isLegalMove[0], numLegalMoves);
}

Node::Node(const State& gameState, Node* parent, MoveEnum move) : gameState(gameState), parent(parent) {
  this->gameState.performMove(move);

  // Populate legal moves
  this->gameState.getLegalMoves(&isLegalMove[0], numLegalMoves);
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
  double uct = (numVisits - sumScore) / numVisits + MCTS_C * sqrt(log(parentVisits) / numVisits);
  return uct;
}

Node* Node::getBestChild() const {
  if (numVisits < MTCS_MIN_VISITS) {
    // Select randomly if the Node has not been visited often enough
    vector<Node*> candidates;
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (children[k]) {
        candidates.push_back(children[k]);
      }
    }

    return candidates[Random::rand(candidates.size())];
  }

  Node* bestChild = nullptr;
  double bestScore = -1e9;
  for (int k = 0; k < NUM_MOVES; ++k) {
    if (!children[k]) {
      continue;
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
  assert(bestMove != -1);
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
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!cur->isLegalMove[k] || cur->children[k]) {
        continue;
      }

      Node* child = new Node(cur->gameState, cur, static_cast<MoveEnum>(k));
      assert(child != nullptr);
      cur->children[k] = child;
      cur->numChildren++;
      cur = child;

      break;
    }
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
  double result = tmpState.getResult();
  if (cur->gameState.playerToMove == 1) {
    result = 1 - result;
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

  int numLegalMoves;
  bool isLegalMove[NUM_MOVES];
  state.getLegalMoves(isLegalMove, numLegalMoves);

  vector<MoveEnum> legalMoves;
  for (int k = 0; k < NUM_MOVES; ++k) {
    if (isLegalMove[k]) {
      legalMoves.push_back(static_cast<MoveEnum>(k));
    }
  }
  assert(!legalMoves.empty());
  return legalMoves[Random::rand(legalMoves.size())];
}
