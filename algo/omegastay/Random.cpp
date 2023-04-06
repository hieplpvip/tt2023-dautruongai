// https://prng.di.unimi.it/xoshiro128plus.c

#include "Random.h"

static inline uint32_t rotl(const uint32_t x, int k) {
  return (x << k) | (x >> (32 - k));
}

static uint32_t s[4];

namespace Random {
  void seed(uint64_t x) {
    // Generate seed using SplitMix64
    for (int i = 0; i < 4; ++i) {
      uint64_t z = (x += 0x9e3779b97f4a7c15);
      z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
      z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
      s[i] = z ^ (z >> 31);
    }
  }

  uint32_t next() {
    const uint32_t result = s[0] + s[3];
    const uint32_t t = s[1] << 9;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = rotl(s[3], 11);
    return result;
  }

  uint32_t rand(uint32_t l, uint32_t r) {
    return l + next() % (r - l + 1);
  }

  uint32_t rand(uint32_t n) {
    return rand(0, n - 1);
  }
}
