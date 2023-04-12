#ifndef MCTS_H
#define MCTS_H

#include "State.h"
#include "Utility.h"
#include <vector>

namespace MCTS {
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

    // Values used for fast calculation of UCT
    double winRate = 0, sqrtLogNumVisits = 0, CDivSqrtNumVisits = 0;

    // Cached value of gameState.isTerminal()
    bool isTerminal = false;

    // Check if the node is fully expanded (i.e. all its children have been created)
    bool isFullyExpanded() const;

    /*
     * Return the UCT score of a node from the perspective
     * of the player who just has moved.
     */
    double getUCT() const;

    /*
     * Return the best child of the node
     * depth is the depth of the node in the tree
     */
    Node* getBestChild(int depth) const;
  };

  // Create a new root node from the root state, not avoid going back
  Node* newRootNode();

  // Create a new node from the given game state and parent node,
  // after performing the given move
  // depth is the depth of parent
  Node* newNode(const State& gameState, Node* parent, MoveEnum move, int depth);

  // Create a new node from the given game state
  Node* newStartNode(const State& gameState);

  // Create a new node from the given game state and parent node,
  // and set the start position.
  // For use in Engine::findStartingPosition()
  Node* newStartNode(const State& gameState, Node* parent, Position startPos);

  // Print statistics of the tree
  void printStats(const Node* root);

  // Return the best move after performing numIterations iterations of MCTS
  MoveEnum findBestMove(Node* root, int numIterations);

  // Perform an iteration of Monte Carlo tree search
  void search(Node* root);

  // Return a random move from the given state
  MoveEnum getRandomMove(State& state, int lastMove);
}

#endif
