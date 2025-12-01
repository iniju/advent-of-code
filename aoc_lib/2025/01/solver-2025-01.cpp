#include <aoc.h>

#include <fmt/format.h>

namespace {}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 01>::solve() -> Result {
  std::vector<i32> rotations;
  i32 i;
  for (auto line : input | std::views::split('\n')) {
    const auto line_start = std::ranges::data(line);
    const auto line_end = line_start + std::ranges::size(line);
    fast_float::from_chars(line_start + 1, line_end, i);
    if (*line_start == 'L') i = -i;
    rotations.push_back(i);
  }

  // Part 1 & Part 2
  u64 part1 = 0, part2 = 0;
  i32 dial = 50, new_dial;
  for (i32 rotation : rotations) {
    new_dial = dial + rotation;
    if (rotation > 0 && new_dial >= 100) {
      part2 += new_dial / 100;
    } else if (new_dial <= 0) {
      part2 += std::abs(new_dial / 100);
      if (dial) part2++;
    }
    dial = (new_dial + 10000) % 100;
    if (!dial) part1++;
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 01>::PartOne() -> std::string { return "995"; }

template<> auto advent<2025, 01>::PartTwo() -> std::string { return "5847"; }
