#include <aoc.hpp>

namespace {

using Grid = std::vector<absl::string_view>;

bool FindWordXMAS(const Grid &grid, const i64 i, const i64 j, const i64 di, const i64 dj) {
  return grid[i][j] == 'X' && grid[i + di][j + dj] == 'M' && grid[i + 2 * di][j + 2 * dj] == 'A'
      && grid[i + 3 * di][j + 3 * dj] == 'S';
}
bool FindWordMAS(const Grid &grid, const i64 mi, const i64 mj, const i64 si, const i64 sj) {
  return grid[mi][mj] == 'M' && grid[si][sj] == 'S';
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 04>::solve() -> Result {
  const Grid grid = absl::StrSplit(input, '\n');

  const u64 height = grid.size();
  const u64 width = grid[0].size();

  // Part 1
  u64 part1 = 0;
  for (i64 i = 0; i < height; i++) {
    for (i64 j = 0; j < width; j++) {
      for (aoc::Dir8 d : aoc::kAllDir8s) {
        const auto move = aoc::MoveDir8(d);
        const i64 fi = i + 3 * move.i;
        const i64 fj = j + 3 * move.j;
        if (fi < 0 || fi >= height || fj < 0 || fj >= width) continue;
        if (FindWordXMAS(grid, i, j, move.i, move.j)) part1++;
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  for (i64 i = 1; i < height - 1; i++) {
    size_t j = grid[i].find_first_of('A', 1);
    while (j < grid[i].size() - 1) {
      if ((FindWordMAS(grid, i - 1, j - 1, i + 1, j + 1) || FindWordMAS(grid, i + 1, j + 1, i - 1, j - 1)) &&
          (FindWordMAS(grid, i - 1, j + 1, i + 1, j - 1) || FindWordMAS(grid, i + 1, j - 1, i - 1, j + 1))) {
        part2++;
      }
      j = grid[i].find_first_of('A', j + 1);
    }
  }
  return aoc::result(part1, part2);
}

template<> auto advent<2024, 4>::PartOne() -> std::string { return "2414"; }

template<> auto advent<2024, 4>::PartTwo() -> std::string { return "1871"; }
