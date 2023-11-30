#include <aoc.hpp>

auto main() -> int {
  constexpr int year{2023};
  constexpr int day{1};

  auto result = advent<year>{}(day);
  fmt::print("{}/{}: {}\n", year, day, result);
  return 0;
}