#ifndef UTILITY_H
#define UTILITY_H

#define BIT(x, i) (((x) >> (i)) & 1)

/*
 * Prints the final move to the output file.
 * Override the output file if exists.
 *
 * @param x 0-indexed x-coordinate
 * @param y 0-indexed y-coordinate
 */
void printFinalMove(int x, int y);

#endif
