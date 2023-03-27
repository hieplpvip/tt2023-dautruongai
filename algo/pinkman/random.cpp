#include "random.h"

// PRNG
mt19937_64 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

int Random::rand(int l, int r) {
  return uniform_int_distribution<int>(l, r)(rng);
}

int Random::rand(int n) {
  return rand(0, n - 1);
}
