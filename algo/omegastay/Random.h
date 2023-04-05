#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>

namespace Random {
  // Seed the random number generator
  void seed(uint64_t x);

  // Return a random 64-bit integer using xoshiro256+
  uint64_t next();

  // Return a random integer in [l, r] using xoshiro256+
  uint32_t rand(uint32_t l, uint32_t r);

  // Return a random integer in [0, n - 1] using xoshiro256+
  uint32_t rand(uint32_t n);
};

#endif
