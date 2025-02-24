#ifndef ADVENTOFCODE_AOC_HPP
#define ADVENTOFCODE_AOC_HPP

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <chrono>
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
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/log/check.h>
#include <absl/hash/hash.h>
#include <absl/strings/str_split.h>
#include <absl/strings/substitute.h>

#include <Eigen/Core>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <scn/scan.h>

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

  Result DoSolve() {
    GetInput();
    return solve();
  }

  void print() {
    GetInput();
    auto start = std::chrono::high_resolution_clock::now();
    const auto [part1, part2] = solve();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    fmt::print("{}/{:02d} -> Part 1: {:20}\tPart 2: {:20}\t\t{:>10.5f} ms\n",
               year,
               day,
               part1,
               part2,
               (long double) duration_nanos / 1.0e6l);
  }

  auto PartOne() -> std::string;

  auto PartTwo() -> std::string;

 private:
  std::string input;

  auto solve() -> Result;

  void GetInput(bool example = false, int example_index = 1) {
    std::string path =
        fmt::format("./source/{}/{:02}/{}.txt", year, day, example ? fmt::format("example{}", example_index) : "input");
    std::ifstream f(path);
    CHECK(f.is_open()) << fmt::format("Could not open path {}\n", path) << std::endl;
    std::stringstream ss;
    ss << f.rdbuf();
    std::string result = ss.str();
    result = absl::StripTrailingAsciiWhitespace(result);
    input = result;
  }
};

namespace aoc {

struct Pos {
  i64 i;
  i64 j;
  inline Pos() : i(0), j(0) {}
  inline Pos(i64 _i, i64 _j) : i(_i), j(_j) {}
  inline Pos(const Pos &o) = default;
  inline Pos(absl::string_view str) {
    auto result = scn::scan<i64, i64>(str, "{},{}");
    CHECK(result) << "can't parse '" << str << "'.";
    std::tie(i, j) = result->values();
  }
  inline bool operator==(const Pos &r) const { return i == r.i && j == r.j; }
  inline Pos operator+(const Pos &r) const { return {i + r.i, j + r.j}; }
  inline Pos operator-(const Pos &r) const { return {i - r.i, j - r.j}; }
  inline Pos operator*(i64 x) const { return {i * x, j * x}; }
  inline bool operator<(const Pos &r) const { return i < r.i || (i == r.i && j < r.j); }
  inline std::string toString() const { return fmt::format("{},{}", i, j); }
  template<typename H>
  friend H AbslHashValue(H h, const Pos &p) {
    return H::combine(std::move(h), p.i, p.j);
  }
};
enum class Dir : char {
  N = '^',
  E = '>',
  S = 'v',
  W = '<',
};
inline Dir ParseDir(char ch) {
  switch (ch) {
    case '^': return Dir::N;
    case '>': return Dir::E;
    case 'v': return Dir::S;
    case '<': return Dir::W;
    default: CHECK(false) << "Unknown Dir representation '" << ch << "'.";
  }
}
static constexpr std::array<Dir, 4> kAllDirs{Dir::N, Dir::E, Dir::S, Dir::W};
inline Pos MoveDir(Dir dir) {
  static std::array<Pos, 4> kDirMoves{{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};
  switch (dir) {
    case Dir::N: return kDirMoves[0];
    case Dir::E: return kDirMoves[1];
    case Dir::S: return kDirMoves[2];
    case Dir::W: return kDirMoves[3];
  }
}
inline Dir OppositeDir(Dir dir) {
  switch (dir) {
    case Dir::N: return Dir::S;
    case Dir::E: return Dir::W;
    case Dir::S: return Dir::N;
    case Dir::W: return Dir::E;
  }
}
inline Dir TurnLeft(Dir dir) {
  switch (dir) {
    case Dir::N: return Dir::W;
    case Dir::E: return Dir::N;
    case Dir::S: return Dir::E;
    case Dir::W: return Dir::S;
  }
}
inline Dir TurnRight(Dir dir) {
  switch (dir) {
    case Dir::N: return Dir::E;
    case Dir::E: return Dir::S;
    case Dir::S: return Dir::W;
    case Dir::W: return Dir::N;
  }
}

enum class Dir8 : char {
  N = '^',
  NE = '7',
  E = '>',
  SE = 'J',
  S = 'v',
  SW = 'L',
  W = '<',
  NW = 'F',
};
static constexpr std::array<Dir8, 8>
    kAllDir8s{Dir8::N, Dir8::NE, Dir8::E, Dir8::SE, Dir8::S, Dir8::SW, Dir8::W, Dir8::NW};
inline Pos MoveDir8(Dir8 dir) {
  static std::array<Pos, 8> kDir8Moves{{{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}}};
  switch (dir) {
    case Dir8::N: return kDir8Moves[0];
    case Dir8::NE: return kDir8Moves[1];
    case Dir8::E: return kDir8Moves[2];
    case Dir8::SE: return kDir8Moves[3];
    case Dir8::S: return kDir8Moves[4];
    case Dir8::SW: return kDir8Moves[5];
    case Dir8::W: return kDir8Moves[6];
    case Dir8::NW: return kDir8Moves[7];
  }
}
template<typename T1, typename T2>
auto result(T1 t1, T2 t2) -> std::tuple<std::string, std::string> {
  return std::tuple{fmt::format("{}", t1), fmt::format("{}", t2)};
}

template<typename E>
auto ToUnderlying(E e) { return static_cast<std::underlying_type_t<E>>(e); }

inline i32 signum(i32 val) {
  return (0 < val) - (val < 0);
}

static constexpr std::array<u64, 20> kTenPowers{
    1u,
    10u,
    100u,
    1'000u,
    10'000u,
    100'000u,
    1'000'000u,
    10'000'000u,
    100'000'000u,
    1'000'000'000u,
    10'000'000'000u,
    100'000'000'000u,
    1'000'000'000'000u,
    10'000'000'000'000u,
    100'000'000'000'000u,
    1'000'000'000'000'000u,
    10'000'000'000'000'000u,
    100'000'000'000'000'000u,
    1'000'000'000'000'000'000u,
    10'000'000'000'000'000'000u};

namespace util {

inline bool IsInMap(u64 height, u64 width, const Pos &pos) {
  return pos.i >= 0 && pos.i < height && pos.j >= 0 && pos.j < width;
}
inline bool IsOutOfMap(u64 height, u64 width, const aoc::Pos &pos) {
  return pos.i < 0 || pos.i >= height || pos.j < 0 || pos.j >= width;
}

template<typename T>
void ScanList(absl::string_view input, std::vector<T> &list, absl::string_view separator = " ") {
  absl::c_transform(absl::StrSplit(input, separator), std::back_inserter(list), [](auto token) {
    auto result = scn::scan<T>(token, "{}");
    CHECK(result) << "Can't parse token '" << token << "'.";
    return result->value();
  });
}

template<typename T>
std::vector<T> TokenizeInput(absl::string_view input,
                             std::function<T(absl::string_view)> transform,
                             absl::string_view separator = "\n") {
  std::vector<absl::string_view> tokens = absl::StrSplit(input, separator, absl::SkipWhitespace());
  std::vector<T> result;
  result.reserve(tokens.size());
  absl::c_transform(tokens, std::back_inserter(result), transform);
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

template<typename UType>
absl::flat_hash_map<UType, u64> Factorize(UType x, const std::vector<UType> &primes) {
  absl::flat_hash_map<UType, u64> result{};
  UType rem{x};
  int pi = 0;
  while (rem != 1 && pi < primes.size()) {
    UType prime = primes[pi];
    while (rem % prime == 0) {
      result[prime]++;
      rem /= prime;
    }
    pi++;
  }
  CHECK(rem == 1) << "Need more primes.";
  return result;
}

template<typename UType>
UType LCM(absl::flat_hash_set<UType> values) {
  UType lcm = 1;
  u64 max_value = *absl::c_max_element(values);
  std::vector<UType> primes = aoc::util::PrimeSieve(max_value + 1);
  absl::flat_hash_map<UType, u64> common_factors{};
  for (UType value : values) {
    auto factors = Factorize(value, primes);
    for (auto [prime, power] : factors) {
      if (common_factors[prime] < power) common_factors[prime] = power;
    }
  }
  for (auto [prime, power] : common_factors) {
    for (int i = 0; i < power; i++) {
      lcm *= prime;
    }
  }
  return lcm;
}

template<typename T>
u32 NumDigits(T x);
// partial specialization optimization for 64-bit numbers
template<>
u32 NumDigits(u64 x);
// partial specialization optimization for 32-bit numbers
template<>
u32 NumDigits(u32 x);

struct EigenMatrixHashWrapper {
  Eigen::MatrixXi m;
};

template<typename H>
H AbslHashValue(H h, const EigenMatrixHashWrapper &m) {
  H state = H::combine(std::move(h), m.m.rows(), m.m.cols());
  for (i32 i = 0; i < m.m.rows(); i++) {
    for (i32 j = 0; j < m.m.cols(); j++) {
      state = H::combine(std::move(state), m.m(i, j));
    }
  }
  return state;
}
using EigenMatrixHasher = absl::Hash<EigenMatrixHashWrapper>;

}  // util

}  // namespace aoc


//namespace std {
//
//template<typename Scalar, int Rows, int Cols>
//struct hash<Eigen::Matrix<Scalar, Rows, Cols>> {
// https://wjngkoh.wordpress.com/2015/03/04/c-hash-function-for-eigen-matrix-and-vector/
//  size_t operator()(const Eigen::Matrix<Scalar, Rows, Cols> &matrix) const {
//    size_t seed = 0;
//    for (size_t i = 0; i < matrix.size(); ++i) {
//      Scalar elem = *(matrix.data() + i);
//      seed ^= std::hash<Scalar>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//    }
//    return seed;
//  }
//};
//
//}  // namespace std

namespace fmt {

template<>
struct formatter<aoc::Dir> : formatter<char> {
  auto format(aoc::Dir dir, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(dir), ctx);
  }
};
template<>
struct formatter<aoc::Dir8> : formatter<char> {
  auto format(aoc::Dir8 dir, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(dir), ctx);
  }
};

template<>
struct formatter<aoc::Pos> : formatter<string_view> {
  inline auto format(const aoc::Pos &p, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("[{}, {}]", p.i, p.j), ctx);
  }
};

}  // namespace fmt

#endif // ADVENTOFCODE_AOC_HPP
