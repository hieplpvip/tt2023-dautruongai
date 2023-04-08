#ifndef STORE_H
#define STORE_H

#include "Constants.h"
#include "State.h"
#include <vector>

/**
 * Persistent store for program state.
 */
namespace Store {
  // Number of rows
  extern int M;

  // Number of columns
  extern int N;

  // Number of turns
  extern int K;

  // Half the number of turns
  extern int HALF_K;

  // Current turn
  extern int currentTurn;

  /*
   * dist[shield] is all-pairs shortest path between cells.
   * shield = 0 means we **DO NOT** have shield
   * shield = 1 means we DO** have shield
   * INF means no path
   */
  extern int dist[2][15][15][15][15];

  /*
   * numLegalMovesNoShield[shield][i][j] is number of legal moves from cell (i, j)
   * shield = 0 means we **DO NOT** have shield
   * shield = 1 means we DO** have shield
   */
  extern int numLegalMoves[2][15][15];

  /*
   * isLegalMoveNoShield[shield][i][j][k] is true if we can move from cell (i, j) to cell (i + dx[k], j + dy[k])
   * shield = 0 means we **DO NOT** have shield
   * shield = 1 means we DO** have shield
   */
  extern bool isLegalMove[2][15][15][NUM_MOVES];

  // Past game states
  extern std::vector<State> pastStates;

  // Load store from STATE.OUT
  void load();

  // Save store to STATE.OUT
  void save();
};

#endif
