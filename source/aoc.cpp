#include "aoc.hpp"

namespace aoc::util {

template<typename T>
u32 NumDigits(T x) {
  u32 digits = 0;
  while (x) {
    x /= 10;
    digits++;
  }
  return digits;
}

// partial specialization optimization for 64-bit numbers
template<>
u32 NumDigits(u64 x) {
  if (x >= 10000000000) {
    if (x >= 100000000000000) {
      if (x >= 10000000000000000) {
        if (x >= 100000000000000000) {
          if (x >= 1000000000000000000) {
            if (x >= 10000000000000000000u) {
              return 20;
            }
            return 19;
          }
          return 18;
        }
        return 17;
      }
      if (x >= 1000000000000000)
        return 16;
      return 15;
    }
    if (x >= 1000000000000) {
      if (x >= 10000000000000)
        return 14;
      return 13;
    }
    if (x >= 100000000000)
      return 12;
    return 11;
  }
  if (x >= 100000) {
    if (x >= 10000000) {
      if (x >= 100000000) {
        if (x >= 1000000000)
          return 10;
        return 9;
      }
      return 8;
    }
    if (x >= 1000000)
      return 7;
    return 6;
  }
  if (x >= 100) {
    if (x >= 1000) {
      if (x >= 10000)
        return 5;
      return 4;
    }
    return 3;
  }
  if (x >= 10)
    return 2;
  return 1;
}

// partial specialization optimization for 32-bit numbers
template<>
u32 NumDigits(u32 x) {
  if (x >= 10'000) {
    if (x >= 10'000'000) {
      if (x >= 100'000'000) {
        if (x >= 1'000'000'000)
          return 10;
        return 9;
      }
      return 8;
    }
    if (x >= 100000) {
      if (x >= 1000000)
        return 7;
      return 6;
    }
    return 5;
  }
  if (x >= 100) {
    if (x >= 1000)
      return 4;
    return 3;
  }
  if (x >= 10)
    return 2;
  return 1;
}

} // namespace aoc::util

