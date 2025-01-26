#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Grid = std::vector<absl::string_view>;
std::string goal1 = "XMAS";
std::string goal2 = "MAS";

bool FindWord(const Grid &grid, absl::string_view word, i64 i, i64 j, aoc::Dir8 dir) {
  auto move = aoc::MoveDir8(dir);
  for (i64 s = 0; s < word.size(); s++) {
    if (word[s] != grid[i + s * move.i][j + s * move.j]) return false;
  }
  return true;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 04>::solve() -> Result {
  Grid grid = absl::StrSplit(input, "\n", absl::SkipWhitespace());

  u64 height = grid.size();
  u64 width = grid.at(0).size();

  // Part 1
  u64 part1 = 0;
  for (i64 i = 0; i < height; i++) {
    for (i64 j = 0; j < width; j++) {
      for (aoc::Dir8 d : aoc::kAllDir8s) {
        auto move = aoc::MoveDir8(d);
        i64 fi = i + 3 * move.i;
        i64 fj = j + 3 * move.j;
        if (fi < 0 || fi >= height || fj < 0 || fj >= width) continue;
        if (FindWord(grid, goal1, i, j, d)) part1++;
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  for (i64 i = 0; i < height - 2; i++) {
    for (i64 j = 0; j < width - 2; j++) {
      // Early exit if central spot is not an 'A'.
      if (grid.at(i + 1).at(j + 1) != goal2.at(1)) continue;
      if ((FindWord(grid, goal2, i, j, aoc::Dir8::SE) || FindWord(grid, goal2, i + 2, j + 2, aoc::Dir8::NW)) &&
          (FindWord(grid, goal2, i, j + 2, aoc::Dir8::SW) || FindWord(grid, goal2, i + 2, j, aoc::Dir8::NE))) {
        part2++;
      }
    }
  }
  return aoc::result(part1, part2);
}