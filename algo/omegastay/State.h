#ifndef STATE_H
#define STATE_H

#include "Constants.h"
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

  // Last moves of players
  MoveEnum lastMove[2];

  // Position of players
  Position pos[2];

  // Gold collected by players
  int gold[2];

  // Whether players have shield
  bool hasShield[2];

  // Whether players are alive
  bool alive[2];

  /*
   * Check if the state is terminal:
   * - playerToMove == 0 (i.e. both players have moved)
   * - otherwise following rules
   */
  bool isTerminal() const;

  /*
   * Get the result of the game from perspective of first player.
   * This function must be called only if the state is terminal.
   *
   * @return 0 if draw, 1 if first player wins, -1 if second player wins
   */
  int getResult() const;

  /*
   * Perform a move.
   *
   * This function must be called for EACH PLAYER.
   * turnLeft is decremented only after the second player has moved.
   * This is slightly different from the rule, which specify that both players move at the same time.
   * We split into two moves for MCTS/Minimax to work.
   */
  void performMove(MoveEnum move);
};

#endif
