#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Grid = std::vector<absl::string_view>;

inline bool FindWordXMAS(const Grid &grid, i64 i, i64 j, i64 di, i64 dj) {
  return grid[i][j] == 'X' && grid[i + di][j + dj] == 'M' && grid[i + 2 * di][j + 2 * dj] == 'A'
      && grid[i + 3 * di][j + 3 * dj] == 'S';
}
inline bool FindWordMAS(const Grid &grid, i64 mi, i64 mj, i64 si, i64 sj) {
  return grid[mi][mj] == 'M' && grid[si][sj] == 'S';
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 04>::solve() -> Result {
  Grid grid = absl::StrSplit(input, '\n');

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
        if (FindWordXMAS(grid, i, j, move.i, move.j)) part1++;
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  for (i64 i = 1; i < height - 1; i++) {
    size_t j = grid.at(i).find_first_of('A', 1);
    while (j < grid.at(i).size() - 1) {
      if ((FindWordMAS(grid, i - 1, j - 1, i + 1, j + 1) || FindWordMAS(grid, i + 1, j + 1, i - 1, j - 1)) &&
          (FindWordMAS(grid, i - 1, j + 1, i + 1, j - 1) || FindWordMAS(grid, i + 1, j - 1, i - 1, j + 1))) {
        part2++;
      }
      j = grid.at(i).find_first_of('A', j + 1);
    }
  }
  return aoc::result(part1, part2);
}