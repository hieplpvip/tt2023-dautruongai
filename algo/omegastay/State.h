#ifndef STATE_H
#define STATE_H

#include "Constants.h"
#include "Utility.h"
#include <cmath>

struct Position {
  int x;
  int y;

  Position() {}
  Position(int _x, int _y) : x(_x), y(_y) {}

  inline bool isAdjacent(const Position &other) const {
    return abs(x - other.x) + abs(y - other.y) == 1;
  }

  inline bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
  }

  inline bool operator!=(const Position &other) const {
    return x != other.x || y != other.y;
  }

  inline bool operator<(const Position &other) const {
    return x < other.x || (x == other.x && y < other.y);
  }
};

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
  PlayerEnum playerToMove;

  // Last position of players
  Position lastPos[2];

  // Position of players
  Position pos[2];

  // Gold collected by players
  int gold[2];

  // Whether players have shield
  bool hasShield[2];

  // Whether players are eliminated
  bool eliminated[2];

  /*
   * Check if the state is terminal:
   * - playerToMove == ME (i.e. both players have moved)
   * - otherwise following rules
   */
  bool isTerminal() const;

  /*
   * Get the result of the game from perspective of first player.
   * This function can be called only when the state is terminal.
   *
   * @return 1 if a win, 0 if a draw, -1 if a loss.
   */
  int getResult() const;

  /*
   * Get the score of the game from perspective of first player.
   * This function can be called only when the state is terminal.
   *
   * @return gold[0] - gold[1]
   */
  int getScore() const;

  /*
   * Get legal moves for the current player.
   */
  void getLegalMoves(bool *isLegalMove, int &numLegalMoves) const;

  /*
   * Perform a move.
   *
   * This function must be called for EACH PLAYER.
   * turnLeft is decremented only after the second player has moved.
   * This is slightly different from the rule, which specify that both players move at the same time.
   * We split into two moves for MCTS/Minimax to work.
   */
  void performMove(MoveEnum move);

  /*
   * Print state for debugging.
   * This function will do nothing if macro ENABLE_LOGGING is not defined.
   */
  void printState() const;
};

#endif
