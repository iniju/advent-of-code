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

template<int YEAR>
struct advent {
  static constexpr int year = YEAR;
  using result = std::tuple<std::string, std::string>;

  auto operator()(int day) const -> result {
    return days[day - 1]();
  }

  static auto day01() -> result;

  static auto day02() -> result;

  static auto day03() -> result;

  static auto day04() -> result;

  static auto day05() -> result;

  static auto day06() -> result;

  static auto day07() -> result;

  static auto day08() -> result;

  static auto day09() -> result;

  static auto day10() -> result;

  static auto day11() -> result;

  static auto day12() -> result;

  static auto day13() -> result;

  static auto day14() -> result;

  static auto day15() -> result;

  static auto day16() -> result;

  static auto day17() -> result;

  static auto day18() -> result;

  static auto day19() -> result;

  static auto day20() -> result;

  static auto day21() -> result;

  static auto day22() -> result;

  static auto day23() -> result;

  static auto day24() -> result;

  static auto day25() -> result;

 private:
  static constexpr std::array<result (*)(), 6> days = {
      // NOLINT
      &advent<year>::day01,
      &advent<year>::day02,
      &advent<year>::day03,
      &advent<year>::day04,
      &advent<year>::day05,
      &advent<year>::day06,
//      &advent<year>::day07,
//      &advent<year>::day08,
//      &advent<year>::day09,
//      &advent<year>::day10,
//      &advent<year>::day11,
//      &advent<year>::day12,
//      &advent<year>::day13,
//      &advent<year>::day14,
//      &advent<year>::day15,
//      &advent<year>::day16,
//      &advent<year>::day17,
//      &advent<year>::day18,
//      &advent<year>::day19,
//      &advent<year>::day20,
//      &advent<year>::day21,
//      &advent<year>::day22,
//      &advent<year>::day23,
//      &advent<year>::day24,
//      &advent<year>::day25,
  };
};

using advent2020 = advent<2020>;
using advent2023 = advent<2023>;

namespace aoc {
template<typename T1, typename T2>
auto result(T1 t1, T2 t2) -> std::tuple<std::string, std::string> {
  return std::tuple{fmt::format("{}", t1), fmt::format("{}", t2)};
}

namespace util {
inline auto GetInput(int year, int day, bool example = false, int example_index = 1) {
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

template<typename T>
std::vector<T> TokenizeInput(absl::string_view input,
                             std::function<T(absl::string_view)> transform,
                             absl::string_view separator = "\n") {
  std::vector<absl::string_view> tokens =
      absl::StrSplit(input, separator, absl::SkipWhitespace());
  auto result = lz::map(tokens, transform).toVector();
  return result;
}

}  // util

}  // namespace aoc

#endif //ADVENTOFCODE_AOC_HPP
