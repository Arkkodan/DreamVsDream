#include "rng.h"

#include <cstdlib>

// RANDOM NUMBER GENERATOR
// Later on, we'll phase out using rand() because it sucks
int util::roll(int size_) {
  return (int)(rollf() * size_); // This is more accurate than using modulus
}

int util::roll(int min_, int max_) { return roll(max_ - min_ + 1) + min_; }

// Random number where 0 <= x < 1
float util::rollf() {
  // ugly sunovabich
  return (float)((double)rand() / (double)((long long)RAND_MAX + 1L));
}
