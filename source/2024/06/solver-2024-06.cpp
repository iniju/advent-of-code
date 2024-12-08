#include <aoc.hpp>

#include <fmt/format.h>

namespace {

enum MapPos : char {
  EMPTY = '.',
  BLOCKED = '#',
};

using Row = std::vector<MapPos>;
using Map = std::vector<Row>;
using PosDir = std::pair<aoc::Pos, aoc::Dir>;
using Visited = absl::flat_hash_set<aoc::Pos>;
using Path = absl::flat_hash_set<PosDir>;

bool TryMove(const Map &map, u64 height, u64 width, aoc::Pos &pos, aoc::Dir &dir) {
  while (true) {
    auto new_pos = pos + aoc::MoveDir(dir);
    if (aoc::util::IsOutOfMap(height, width, new_pos)) return false;
    if (map.at(new_pos.i).at(new_pos.j) != MapPos::BLOCKED) {
      pos = new_pos;
      return true;
    }
    dir = aoc::TurnRight(dir);
  }
}

Visited WalkMap(const Map &map, u64 height, u64 width, const aoc::Pos &start) {
  u64 result = 1;
  aoc::Pos pos = start;
  aoc::Dir dir = aoc::Dir::N;
  Visited visited;
  visited.insert(pos);
  while (TryMove(map, height, width, pos, dir)) {
    if (aoc::util::IsOutOfMap(height, width, pos)) break;
    if (visited.insert(pos).second) {
      result++;
    }
  }
  return visited;
}

bool DetectLoop(const Map &map, u64 height, u64 width, const aoc::Pos &start, const Path &visited, aoc::Dir dir) {
  aoc::Pos pos = start;
  Path path{visited};
  while (TryMove(map, height, width, pos, dir)) {
    if (!path.insert({pos, dir}).second) return true;
  }
  return false;
}

u64 WalkMapWithLoops(Map &map, u64 height, u64 width, const aoc::Pos &start) {
  u64 result = 0;
  aoc::Pos pos = start;
  aoc::Dir dir = aoc::Dir::N;
  Path visited;
  absl::flat_hash_set<aoc::Pos> walked;
  visited.insert({pos, dir});
  walked.insert(pos);
  while (TryMove(map, height, width, pos, dir)) {
    if (aoc::util::IsOutOfMap(height, width, pos)) break;
    visited.insert({pos, dir});
    walked.insert(pos);
    // Try block in front.
    auto block = pos + aoc::MoveDir(dir);
    if (aoc::util::IsOutOfMap(height, width, block)) {
      // Escaping map, never mind.
      continue;
    }
    if (map.at(block.i).at(block.j) == MapPos::BLOCKED) {
      // Front is already blocked, anticipate turn to the right and block the right instead.
      // If the right is also blocked (checked later), there's no point trying further right, the guard just walked in
      // from there.
      block = pos + aoc::MoveDir(aoc::TurnRight(dir));
      if (aoc::util::IsOutOfMap(height, width, block)) {
        // Escaping map, never mind.
        continue;
      }
    }
    if (map.at(block.i).at(block.j) == MapPos::EMPTY && !walked.contains(block)) {
      map[block.i][block.j] = MapPos::BLOCKED;
      if (DetectLoop(map, height, width, pos, visited, aoc::TurnRight(dir))) result++;
      map[block.i][block.j] = MapPos::EMPTY;
    }
  }
  return result;
}

}  // namespace

namespace fmt {

//template<>
//struct formatter<MapPos> : formatter<char> {
//  auto format(MapPos pos, format_context &ctx) const {
//    return formatter<char>::format(aoc::ToUnderlying(pos), ctx);
//  }
//};
//template<>
//struct formatter<std::vector<MapPos>> : formatter<string_view> {
//  auto format(std::vector<MapPos> row, format_context &ctx) const {
//    return formatter<string_view>::format(fmt::format("{}", join(row, "")), ctx);
//  }
//};
//template<>
//struct formatter<Map> : formatter<string_view> {
//  auto format(const Map &map, format_context &ctx) const {
//    return formatter<string_view>::format(fmt::format("{}", join(map, "\n")), ctx);
//  }
//};

}  // namespace fmt

template<>
auto advent<2024, 06>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u64 height = lines.size();
  u64 width = lines.at(0).size();
  Map map;
  aoc::Pos guard;
  for (u64 i = 0; i < height; i++) {
    Row row;
    for (u64 j = 0; j < width; j++) {
      switch (lines.at(i).at(j)) {
        case '.':row.push_back(MapPos::EMPTY);
          break;
        case '#':row.push_back(MapPos::BLOCKED);
          break;
        case '^':row.push_back(MapPos::EMPTY);
          guard.i = static_cast<i64>(i);
          guard.j = static_cast<i64>(j);
          break;
        default: CHECK(false) << "Unexpected map character: '" << lines.at(i).at(j) << "'.";
      }
    }
    map.push_back(row);
  }

  // Part 1
  auto visited = WalkMap(map, height, width, guard);
  u64 part1 = visited.size();

  // Part 2
  u64 part2 = WalkMapWithLoops(map, height, width, guard);

  return aoc::result(part1, part2);
}