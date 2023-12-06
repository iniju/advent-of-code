#include <aoc.hpp>
#include <chrono>

auto main() -> int {
  constexpr int year{2023};

  for (int day = 1; day <= 6; day++) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = advent<year>{}(day);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    fmt::print("{}/{:02d} -> {} ({:0.5f} sec)\n", year, day, result, (long double) duration_nanos / 1.0e9l);
  }
  return 0;
}