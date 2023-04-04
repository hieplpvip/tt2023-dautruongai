#ifndef MCTS_H
#define MCTS_H

#include "State.h"
#include "Utility.h"
#include <vector>

struct MonteCarloTreeSearch {
  struct Node {
    // Game state corresponding to this node
    State gameState;

    // Parent node
    Node* parent = nullptr;

    /*
     * Number of created child nodes
     * numChildren <= numLegalMoves
     */
    int numChildren = 0;
    Node* children[NUM_MOVES] = {};

    // Number of legal moves
    int numLegalMoves = 0;
    bool isLegalMove[NUM_MOVES] = {};

    // Number of visits through this node
    int numVisits = 0;

    // Sum of score of all visits from the perspective of the player who just has moved
    double sumScore = 0;

    // Cached value of gameState.isTerminal()
    bool isTerminal = false;

    // Create a node with the given game state
    Node(const State& gameState);

    // Create a child node by cloning parent and performing the given move
    Node(const State& gameState, Node* parent, MoveEnum move);

    // Check if the node is fully expanded (i.e. all its children have been created)
    bool isFullyExpanded() const;

    /*
     * Return the UCT score of a node from the perspective
     * of the player who just has moved.
     */
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
