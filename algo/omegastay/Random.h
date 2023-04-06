#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>

namespace Random {
  // Seed the random number generator
  void seed(uint64_t x);

  // Return a random 32-bit integer using xoshiro128+
  uint32_t next();

  // Return a random integer in [0, n - 1]
  // using xoshiro128+ and Lemire's nearly divisionless method
  uint32_t rand(uint32_t n);

  // Return a random integer in [l, r].
  // This is just a wrapper for l + rand(r - l + 1)
  uint32_t rand(uint32_t l, uint32_t r);
};

#endif
