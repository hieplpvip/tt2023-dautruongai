#ifndef UTILITY_H
#define UTILITY_H

#define isValidPos(x, y) (0 <= x && x < Store::M && 0 <= y && y < Store::N)

namespace Random {
  // Return a random integer in [l, r].
  int rand(int l, int r);

  // Return a random integer in [0, n - 1].
  int rand(int n);
};

struct Position {
  int x;
  int y;

  Position() {}
  Position(int x, int y) : x(x), y(y) {}
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
