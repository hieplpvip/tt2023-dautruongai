#include "Utility.h"
#include <cstdio>
#include <cassert>
#include <random>
#include <chrono>

namespace Random {
  std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

  int rand(int l, int r) {
    return std::uniform_int_distribution<int>(l, r)(rng);
  }

  int rand(int n) {
    return rand(0, n - 1);
  }
}

void printFinalMove(int x, int y) {
  FILE* f = fopen("MOVE.OUT", "w");
  assert(f != NULL);
  fprintf(f, "%d %d\n", x, y);
  fclose(f);
}
