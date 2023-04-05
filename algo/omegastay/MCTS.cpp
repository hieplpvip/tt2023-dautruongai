#include "MCTS.h"
#include "Random.h"
#include "Store.h"
#include "Utility.h"
#include <bits/stdc++.h>

using namespace std;
using Node = MonteCarloTreeSearch::Node;

Node::Node(const State& gameState) : gameState(gameState) {
  this->isTerminal = this->gameState.isTerminal();

  // Populate legal moves
  this->gameState.getLegalMoves(&isLegalMove[0], numLegalMoves);
}

Node::Node(const State& gameState, Node* parent, MoveEnum move) : gameState(gameState), parent(parent) {
  this->gameState.performMove(move);
  this->isTerminal = this->gameState.isTerminal();

  // Populate legal moves
  this->gameState.getLegalMoves(&isLegalMove[0], numLegalMoves);
}

bool Node::isFullyExpanded() const {
  return numChildren == numLegalMoves;
}

double Node::getUCT() const {
  dassert(numVisits > 0);
  double parentVisits = 0.0;
  if (parent != nullptr) {
    parentVisits = parent->numVisits;
  }
  double uct = (double)-sumScore / numVisits + MCTS_C * sqrt(log(parentVisits) / numVisits);
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
  dassert(bestChild != nullptr);
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
  dassert(bestMove != -1);
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
  if (!cur->isTerminal) {
    for (int k = 0; k < NUM_MOVES; ++k) {
      if (!cur->isLegalMove[k] || cur->children[k]) {
        continue;
      }

      Node* child = new Node(cur->gameState, cur, static_cast<MoveEnum>(k));
      dassert(child != nullptr);
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
  int result = tmpState.getResult();
  if (cur->gameState.playerToMove == 1) {
    result = -result;
  }
  do {
    cur->numVisits++;
    cur->sumScore += result;
    cur = cur->parent;
    result = -result;
  } while (cur != nullptr);
}

MoveEnum MonteCarloTreeSearch::getRandomMove(const State& state) const {
  // TODO: use better simulation strategy

  static int prob[NUM_MOVES];
  static int numLegalMoves;
  static bool isLegalMove[NUM_MOVES];
  state.getLegalMoves(isLegalMove, numLegalMoves);

  for (int k = 0; k < NUM_MOVES; ++k) {
    prob[k] = isLegalMove[k];
  }
  for (int k = 1; k < NUM_MOVES; ++k) {
    prob[k] += prob[k - 1];
  }
  dassert(prob[NUM_MOVES - 1] > 0);

  int r = Random::rand(prob[NUM_MOVES - 1]);
  for (int k = 0; k < NUM_MOVES; ++k) {
    if (r < prob[k]) {
      return static_cast<MoveEnum>(k);
    }
  }
  __builtin_unreachable();
}
