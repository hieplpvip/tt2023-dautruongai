#ifndef STATE_H
#define STATE_H

#include "Constants.h"

struct Position {
  int x;
  int y;

  Position() {}
  Position(int _x, int _y) : x(_x), y(_y) {}

  inline bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
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
   * @return 0 if draw, INF if player 1 wins, otherwise -INF.
   * The score will be add into this to encourage player countinuing their move.
   */
  score_t getResult() const;

  /*
   * Get the score of the game from perspective of first player.
   * This function can be called only when the minimax search reach max depth.
   *
   * @return the score (with heuristic) difference between two players.
   */
  score_t getScore();

  /*
   * Perform a move.
   *
   * This function must be called for EACH PLAYER.
   * turnLeft is decremented only after the second player has moved.
   * This is slightly different from the rule, which specify that both players move at the same time.
   * We split into two moves for MCTS/Minimax to work.
   */
  void performMove(PlayerEnum player, MoveEnum move);
};

#endif  // STATE_H
