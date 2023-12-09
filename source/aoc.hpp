#ifndef ADVENTOFCODE_AOC_HPP
#define ADVENTOFCODE_AOC_HPP

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ostream>
#include <queue>
#include <ranges>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/log/check.h>
#include <absl/strings/str_split.h>
#include <absl/strings/substitute.h>

//#include <Eigen/Core>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <scn/scn.h>

#undef LZ_STANDALONE

#include <Lz/Lz.hpp>

// convenience aliases
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

template<int YEAR, int DAY>
struct advent {
  static constexpr int year = YEAR;
  static constexpr int day = DAY;
  using Result = std::tuple<std::string, std::string>;

  static auto solve() -> Result;

  static void print() {
    auto start = std::chrono::high_resolution_clock::now();
    const auto [part1, part2] = solve();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    fmt::print("{}/{:02d} -> Part 1: {:15}\tPart 2: {:15}\t\t({:0.5f} sec)\n",
               year,
               day,
               part1,
               part2,
               (long double) duration_nanos / 1.0e9l);
  }

 private:
  static auto GetInput(bool example = false, int example_index = 1) {
    std::string path =
        fmt::format("./source/{}/{:02}/{}.txt", year, day, example ? fmt::format("example{}", example_index) : "input");
    std::ifstream f(path);
    if (f.is_open()) {
      std::stringstream ss;
      ss << f.rdbuf();
      return ss.str();
    }
    CHECK(false) << fmt::format("Could not open path {}\n", path) << std::endl;
    std::terminate();
  }
};

namespace aoc {
template<typename T1, typename T2>
auto result(T1 t1, T2 t2) -> std::tuple<std::string, std::string> {
  return std::tuple{fmt::format("{}", t1), fmt::format("{}", t2)};
}

namespace util {

template<typename T>
std::vector<T> TokenizeInput(absl::string_view input,
                             std::function<T(absl::string_view)> transform,
                             absl::string_view separator = "\n") {
  std::vector<absl::string_view> tokens =
      absl::StrSplit(input, separator, absl::SkipWhitespace());
  auto result = lz::map(tokens, transform).toVector();
  return result;
}

template<typename UType>
std::vector<UType> PrimeSieve(UType limit) {
  if (limit < 2U) {
    // No primes less than 2
    return std::vector<UType>();
  }
  // An array of bools where sieve[i] is false if i is prime
  std::vector<bool> sieve(limit, false);

  // 0 and 1 are not prime.
  sieve[0] = true;
  sieve[1] = true;

  std::vector<UType> primes;
  for (UType loop = 0; loop < limit; ++loop) {
    if (!sieve[loop]) {
      // loop is prime
      primes.push_back(loop);

      // Mark i*i..limit in sieve as not prime,
      // as they are a multiple of i
      for (UType j = loop * loop; j < limit; j += loop) {
        sieve[j] = true;
      }
    }
  }
  return primes;
}

}  // util

}  // namespace aoc

#endif //ADVENTOFCODE_AOC_HPP
