#ifndef MCTS_H
#define MCTS_H

#include "State.h"
#include "Utility.h"
#include <vector>

struct MonteCarloTreeSearch {
  struct Node {
    State gameState;
    Node* parent = nullptr;
    Node* children[NUM_MOVES] = {};
    int numChildren = 0;
    int numLegalMoves = 0;
    bool isLegalMove[NUM_MOVES] = {};

    int numVisits = 0;
    int sumScore = 0;

    Node(const State& gameState);
    Node(const State& gameState, Node* parent);

    // Check if the node is fully expanded (i.e. all its children have been created)
    bool isFullyExpanded() const;

    // Return the UCT score of a node
    double getUCT() const;

    // Return the best child of the node
    Node* getBestChild() const;
  };

  // The root of the tree
  Node* root;

  MonteCarloTreeSearch(const State& rootState);

  // Return the best move after performing numIterations iterations of MCTS
  MoveEnum findBestMove(int numIterations);

private:
  // Perform an iteration of Monte Carlo tree search
  void search();

  // Return a random move from the given state
  MoveEnum getRandomMove(const State& state) const;
};

#endif
