#ifndef STATE_H
#define STATE_H

#include "constant.h"

#define REPL_MAGNET_CELL(x, y, a, b)                           \
  for (int x = a - MAGNET_RADIUS; x <= a + MAGNET_RADIUS; ++x) \
    for (int y = b - MAGNET_RADIUS; y <= b + MAGNET_RADIUS; ++y)

#define isValidPos(x, y) (0 <= x && x < Store::M && 0 <= y && y < Store::N)

typedef float score_t;

struct Position {
  int x;
  int y;

  Position() {}
  Position(int x, int y) : x(x), y(y) {}

  inline bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
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

  // Last position of players
  Position lastPos[2];

  // Position of players
  Position pos[2];

  // Gold collected by players
  int gold[2];

  // Score of players
  score_t score[2];

  // Whether players have shield
  bool hasShield[2];

  // Whether players are eliminated
  bool eliminated[2];

  /*
   * Check if the state is terminal
   */
  bool isTerminal() const;

  /*
   * Get the result of the game from perspective of first player.
   * This function can be called only when the state is terminal.
   *
   * @return the gold difference between the two players.
   */
  int getResult() const;

  /*
   * Perform a move.
   *
   * This function must be called for EACH PLAYER.
   * This is slightly different from the rule, which specify that both players move at the same time.
   * We split into two moves for MCTS/Minimax to work.
   */
  void performMove(PlayerEnum player, MoveEnum move);

  /*
   * Perform a move with magnet.
   *
   * Similar to performMove, but this will collect all golds in the MAGNET_RADIUS.
   */
  void performMoveWithMagnet(PlayerEnum player, MoveEnum move);
};

extern State rootState;

#endif  // STATE_H