#include <aoc.h>

#include <Eigen/Dense>

namespace {

using Map = Eigen::Array<u32, Eigen::Dynamic, Eigen::Dynamic>;

Map GetFreedomMap(const Map& map, u32 height, u32 width) {
  Map new_map = map;
  new_map.topRows(height - 1) += map.bottomRows(height - 1);
  new_map.bottomRows(height - 1) += map.topRows(height - 1);
  new_map.rightCols(width - 1) += map.leftCols(width - 1);
  new_map.leftCols(width - 1) += map.rightCols(width - 1);
  new_map.topLeftCorner(height - 1, width - 1) += map.bottomRightCorner(height - 1, width - 1);
  new_map.topRightCorner(height - 1, width - 1) += map.bottomLeftCorner(height - 1, width - 1);
  new_map.bottomLeftCorner(height - 1, width - 1) += map.topRightCorner(height - 1, width - 1);
  new_map.bottomRightCorner(height - 1, width - 1) += map.topLeftCorner(height - 1, width - 1);
  new_map *= map;
  return new_map;
}

u64 RemoveAccessible(Map& map) {
  u64 result = (map > 0 && map < 5).count();
  if (result > 0) {
    map = (map > 0 && map < 5).select(0, map);
    map = (map > 0).select(1, map);
  }
  return result;
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 04>::solve() -> Result {
  u32 width = input.find_first_of('\n');
  u32 height = input.size() / width;
  Map map(height, width);
  u32 i = 0, j = 0;
  for (auto c : input) {
    if (c == '\n') {
      i++;
      j = 0;
      continue;
    }
    map(i, j++) = c == '.' ? 0 : 1;
  }

  Map new_map = GetFreedomMap(map, height, width);
  u64 accessible = RemoveAccessible(new_map);
  u64 part1 = accessible;
  u64 part2 = 0;
  while (accessible > 0) {
    part2 += accessible;
    new_map = GetFreedomMap(new_map, height, width);
    accessible = RemoveAccessible(new_map);
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 04>::PartOne() -> std::string { return "1460"; }

template<> auto advent<2025, 04>::PartTwo() -> std::string { return "9243"; }
