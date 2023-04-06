#ifndef UTILITY_H
#define UTILITY_H

#define isValidPos(x, y) (0 <= x && x < Store::M && 0 <= y && y < Store::N)

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
 * Prints the final move to the output file.
 * Override the output file if exists.
 *
 * @param x 0-indexed x-coordinate
 * @param y 0-indexed y-coordinate
 */
void printFinalMove(int x, int y);

#endif
