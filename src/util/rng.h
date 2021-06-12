#ifndef UTIL_RNG_H
#define UTIL_RNG_H

namespace util {

  // RNG

  /// @brief Generate random int
  /// @return Int value ranges from 0 (inclusive) to size (exclusive).
  int roll(int size);

  /// @brief Generate random int
  /// @return Int value ranges from min to max inclusively.
  int roll(int min, int max);

  /// @brief Generate random float
  /// @return Float value ranges from 0.0f (inclusive) to 1.0f (exclusive).
  float rollf();
} // namespace util

#endif // UTIL_RNG_H
