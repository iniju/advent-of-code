#include <aoc.h>

#include <fmt/format.h>

namespace {
using Tile = char;
using Row = std::vector<Tile>;
using Map = std::vector<Row>;
using BoolRow = std::vector<bool>;
using BoolMap = std::vector<BoolRow>;
using Fences = absl::flat_hash_map<aoc::Dir, absl::flat_hash_set<aoc::Pos>>;

std::tuple<u64, u64, u64> ScanPlot(const Map &map, u64 height, u64 width, i64 i, i64 j, BoolMap& scanned) {
  u64 area = 0;
  u64 perimeter = 0;
  Tile tile = map.at(i).at(j);
  std::deque<aoc::Pos> q;
  q.emplace_back(i, j);
  scanned[i][j] = true;
  Fences fences;
  while (!q.empty()) {
    auto pos = q.back();
    q.pop_back();
    area++;
    for (aoc::Dir dir : aoc::kAllDirs) {
      auto new_pos = pos + aoc::MoveDir(dir);
      if (aoc::util::IsOutOfMap(height, width, new_pos)) {
        perimeter++;
        fences[dir].insert(new_pos);
        continue;
      }
      if (map.at(new_pos.i).at(new_pos.j) != tile) {
        perimeter++;
        fences[dir].insert(new_pos);
        continue;
      }
      if (scanned.at(new_pos.i).at(new_pos.j)) continue;
      q.push_front(new_pos);
      scanned[new_pos.i][new_pos.j] = true;
    }
  }
  u64 sides = 0;
  for (auto dir : aoc::kAllDirs) {
    auto& fence = fences[dir];
    while (!fence.empty()) {
      auto start = *fence.begin();
      fence.erase(start);
      sides++;
      // Look left for continuous fence.
      auto left_move = aoc::MoveDir(aoc::TurnLeft(dir));
      auto pos = start;
      while (true) {
        pos = pos + left_move;
        if (!fence.contains(pos)) break;
        fence.erase(pos);
      }
      // Look right for continuous fence.
      auto right_move = aoc::MoveDir(aoc::TurnRight(dir));
      pos = start;
      while(true) {
        pos = pos + right_move;
        if (!fence.contains(pos)) break;
        fence.erase(pos);
      }
    }
  }
  return {area, perimeter, sides};
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 12>::solve() -> Result {
  Map map = aoc::util::TokenizeInput<Row>(input, [](auto line){
    return Row{line.begin(), line.end()};
  });
  u64 height = map.size();
  u64 width = map.at(0).size();

  // Part 1 & Part 2
  u64 part1 = 0;
  u64 part2 = 0;

  BoolMap scanned;
  for (u64 i = 0; i < height; i++) scanned.emplace_back(width, false);

  u64 area, perimeter, sides;
  for (i64 i = 0; i < height; i++) {
    for (i64 j = 0; j < width; j++) {
      if (scanned.at(i).at(j)) continue;
      std::tie(area, perimeter, sides) = ScanPlot(map, height, width, i, j, scanned);
      part1 += area * perimeter;
      part2 += area * sides;
    }
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 12>::PartOne() -> std::string { return "1522850"; }

template<> auto advent<2024, 12>::PartTwo() -> std::string { return "953738"; }
