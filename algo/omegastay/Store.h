#ifndef STORE_H
#define STORE_H

#include "Constants.h"
#include "State.h"
#include <vector>

#define isValidPos(x, y) (0 <= x && x < Store::M && 0 <= y && y < Store::N)

#define REPL_ALL_CELL(x, y)          \
  for (int x = 0; x < Store::M; ++x) \
    for (int y = 0; y < Store::N; ++y)

// Game state input from MAP.INP
extern State rootState;

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

  // Current game phase
  extern GamePhaseEnum gamePhase;

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

  // Game state of previous turn
  extern State pastState;

  // Initialize store (called once at the beginning of a match)
  void init();

  // Update store data (called at the beginning of each turn, starting from turn 2)
  void update();

  // Load store from STATE.OUT
  void load();

  // Save store to STATE.OUT
  void save();
}

#endif
