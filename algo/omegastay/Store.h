#ifndef STORE_H
#define STORE_H

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

  // Current turn
  extern int currentTurn;

  /*
   * All-pairs shortest path between cells when we do not have shield
   * (i.e. can not go through danger cell)
   */
  extern int distNoShield[15][15][15][15];

  /*
   * All-pairs shortest path between cells when we have shield
   * (i.e. can go through danger cell)
   */
  extern int distWithShield[15][15][15][15];

  // Past game states
  extern std::vector<State> pastStates;

  // Load store from STATE.OUT
  void load();

  // Save store to STATE.OUT
  void save();
};

#endif
