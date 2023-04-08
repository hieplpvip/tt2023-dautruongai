#ifndef STORE_H
#define STORE_H

#include "Constants.h"
#include "State.h"

#define REPL_ALL_CELL(x, y)          \
  for (int x = 0; x < Store::M; ++x) \
    for (int y = 0; y < Store::N; ++y)

/*
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
   * All-pairs shortest path between cells
   */
  extern int dist[2][15][15][15][15];

  /*
   * isLegalMove[i][j][k] is true if we can move from cell (i, j) to cell (i + dx[k], j + dy[k])
   */
  extern bool isLegalMove[2][15][15][NUM_MOVES];

  // Past game states
  extern State pastState;

  // Init store
  void init();

  // Update store data
  void update();

  // Load store from STATE.OUT
  void load();

  // Save store to STATE.OUT
  void save();
}

#endif
