#include "Utility.h"
#include <cstdio>
#include <cassert>
#include <random>
#include <chrono>

void printFinalMove(int x, int y) {
  FILE* f = fopen("MOVE.OUT", "w");
  assert(f != NULL);
  fprintf(f, "%d %d\n", x + 1, y + 1);
  fclose(f);
}
