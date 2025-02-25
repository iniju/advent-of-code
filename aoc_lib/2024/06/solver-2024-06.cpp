#include <aoc.hpp>

namespace {

enum Tile : char {
  EMPTY = '.',
  BLOCKED = '#',
};

class Map {
  u64 _height;
  u64 _width;
  std::vector<Tile> _map;
  absl::flat_hash_map<char, i64> _specials;

 public:
  Map(const absl::string_view input,
      const absl::flat_hash_map<char, Tile> &tile_mapping,
      const absl::flat_hash_set<char> &specials) {
    const std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
    _height = lines.size();
    _width = lines[0].size();
    auto joined = absl::StrJoin(lines, "");
    absl::c_transform(joined, std::back_inserter(_map), [&tile_mapping](const char ch) {
      return tile_mapping.at(ch);
    });
    for (char special : specials) _specials[special] = static_cast<i64>(joined.find('^'));
  }
  [[nodiscard]] i64 GetSpecial(const char ch) const {
    return _specials.at(ch);
  }
  [[nodiscard]] i64 MoveDir(const aoc::Dir dir) const {
    switch (dir) {
      case aoc::Dir::N: return static_cast<i64>(-_width);
      case aoc::Dir::E: return 1;
      case aoc::Dir::S: return static_cast<i64>(_width);
      case aoc::Dir::W: return -1;
    }
  }
  bool TryMove(i64 &pos, aoc::Dir &dir, const i64 extra_block = -1) const {
    while (true) {
      const i64 new_pos = pos + MoveDir(dir);
      switch (dir) {
        case aoc::Dir::N:
        case aoc::Dir::S:if (new_pos < 0 || new_pos >= _map.size()) return false;
          break;
        case aoc::Dir::E:
        case aoc::Dir::W:if (new_pos / _width != pos / _width) return false;
          break;
      }
      if (_map[new_pos] != BLOCKED && new_pos != extra_block) {
        pos = new_pos;
        return true;
      }
      dir = aoc::TurnRight(dir);
    }
  }
};

using PosDir = std::pair<i64, aoc::Dir>;
using Visited = std::vector<PosDir>;
using VisitedSet = absl::flat_hash_set<i64>;
using Path = absl::flat_hash_set<PosDir>;

std::tuple<Visited, u64> WalkMap2D(const Map &map, const i64 start) {
  u64 result = 1;
  i64 pos = start;
  auto dir = aoc::Dir::N;
  Visited visited;
  VisitedSet visited_set;
  visited.emplace_back(pos, dir);
  visited_set.insert(pos);
  while (map.TryMove(pos, dir)) {
    if (visited_set.insert(pos).second) {
      result++;
    }
    visited.emplace_back(pos, dir);
  }
  return std::make_tuple(visited, result);
}

bool DetectLoop(const Map &map, const i64 start, const Visited &visited, const u32 visited_limit, aoc::Dir dir, const i64 block) {
  i64 pos = start;
  Path path{visited.begin(), visited.begin() + visited_limit};
  while (map.TryMove(pos, dir, block)) {
    if (!path.insert({pos, dir}).second) return true;
  }
  return false;
}

u64 WalkMapWithLoops(const Map &map, const Visited &visited) {
  std::vector<std::tuple<i64, i64, u32, aoc::Dir>> possible;
  absl::flat_hash_set<i64> walked;
  for (u32 i = 1; i < visited.size(); i++) {
    auto [pos, dir]  = visited[i];
    walked.insert(pos);
    auto block = pos;
    auto block_dir = dir;
    if (!map.TryMove(block, block_dir)) continue;
    if (block_dir == aoc::OppositeDir(dir)) continue;
    if (walked.contains(block)) continue;
    possible.emplace_back(pos, block, i, aoc::TurnRight(dir));
  }
  u64 result = 0;
  #pragma omp parallel for reduction(+:result)
  for (i32 i = 0; i < possible.size(); i++) { // NOLINT(*-loop-convert)
    auto const& [pos, block, vis_i, dir] = possible[i];
    if (DetectLoop(map, pos, visited, vis_i, dir, block)) result++;
  }
  return result;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 6>::solve() -> Result {
  const Map map(input, {{'.', EMPTY}, {'^', EMPTY}, {'#', BLOCKED}}, {'^'});
  const i64 guard = map.GetSpecial('^');

  // Part 1
  auto [visited, part1] = WalkMap2D(map, guard);

  // Part 2
  u64 part2 = WalkMapWithLoops(map, visited);

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 6>::PartOne() -> std::string { return "5329"; }

template<> auto advent<2024, 6>::PartTwo() -> std::string { return "2162"; }
