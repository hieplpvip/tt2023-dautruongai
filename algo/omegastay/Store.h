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
   * All-pairs shortest path between cells when we **DO NOT** have shield
   * (i.e. can not go through danger cell)
   */
  extern int distNoShield[15][15][15][15];

  /*
   * All-pairs shortest path between cells when we **DO** have shield
   * (i.e. can go through danger cell)
   */
  extern int distWithShield[15][15][15][15];

  /*
   * numLegalMovesNoShield[i][j] is number of legal moves from cell (i, j)
   * when we **DO NOT** have shield
   */
  extern int numLegalMovesNoShield[15][15];

  /*
   * numLegalMovesWithShield[i][j] is number of legal moves from cell (i, j)
   * when we DO** have shield
   */
  extern int numLegalMovesWithShield[15][15];

  /*
   * isLegalMoveNoShield[i][j][k] is true if we can move from cell (i, j) to cell (i + dx[k], j + dy[k])
   * when we **DO NOT** have shield
   */
  extern bool isLegalMoveNoShield[15][15][NUM_MOVES];

  /*
   * isLegalMoveWithShield[i][j][k] is true if we can move from cell (i, j) to cell (i + dx[k], j + dy[k])
   * when we **DO** have shield
   */
  extern bool isLegalMoveWithShield[15][15][NUM_MOVES];

  // Past game states
  extern std::vector<State> pastStates;

  // Load store from STATE.OUT
  void load();

  // Save store to STATE.OUT
  void save();
};

#endif
