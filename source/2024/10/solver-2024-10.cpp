#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Altitude = u16;
using Row = std::vector<Altitude>;
using Map = std::vector<Row>;
using Trailheads = std::vector<aoc::Pos>;
using Path = std::pair<std::vector<aoc::Pos>, Altitude>;

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 10>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u64 height = lines.size();
  u64 width = lines.at(0).size();
  Map map;
  Trailheads trailheads;
  for (u32 i = 0; i < height; i++) {
    Row row;
    for (u32 j = 0; j < width; j++) {
      char ch = lines.at(i).at(j);
      row.push_back(ch - '0');
      if (ch == '0') {
        trailheads.emplace_back(i, j);
      }
    }
    map.push_back(row);
  }

  // Part 1 & Part 2
  u64 part1 = 0;
  u64 part2 = 0;
  for (const auto& pos : trailheads) {
    std::deque<Path> paths;
    Path start{{pos}, 0};
    paths.push_back(start);
    absl::flat_hash_set<aoc::Pos> visited;
    absl::flat_hash_set<aoc::Pos> ends;
    absl::flat_hash_set<Path> unique_paths;
    while (!paths.empty()) {
      auto path = paths.back();
      paths.pop_back();
      auto tail = path.first.back();
      for (auto dir : aoc::kAllDirs) {
        auto new_pos = tail + aoc::MoveDir(dir);
        if (aoc::util::IsOutOfMap(height, width, new_pos)) continue;
        auto new_alt = map.at(new_pos.i).at(new_pos.j);
        if (new_alt != path.second + 1) continue;

        Path new_path{path.first, new_alt};
        new_path.first.push_back(new_pos);
        if (new_alt == 9) {
          ends.insert(new_pos);
          unique_paths.insert(new_path);
          continue;
        }
        paths.push_back(new_path);
      }
    }
    part1 += ends.size();
    part2 += unique_paths.size();
  }


  return aoc::result(part1, part2);
}