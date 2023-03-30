#ifndef STATE_H
#define STATE_H

#include "Utility.h"

/*
 * A game state.
 */
struct State {
  /*
   * The current map. Values are encoded as follows:
   * 0: empty cell
   * 101: danger cell
   * 102: shield cell
   * x: gold cell with value x
   */
  unsigned char at[15][15];

  // Number of turns left
  int turnLeft;

  // Player to make the next move
  int playerToMove;

  // Position of players
  Position pos[2];

  // Gold collected by players
  int gold[2];

  // Whether players have shield
  bool hasShield[2];
};

#endif
