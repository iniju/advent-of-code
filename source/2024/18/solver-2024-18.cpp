#include <aoc.hpp>

#include <fmt/format.h>

namespace {

enum class Tile : char {
  SAFE = '.',
  CORRUPTED = '#',
};

using Row = std::vector<Tile>;
using Map = std::vector<Row>;
using Node = std::tuple<i64, i64, aoc::Pos>;
using Path = absl::flat_hash_set<aoc::Pos>;

std::tuple<bool, aoc::Pos> TryMove(const Map &map, u32 height, u32 width, const aoc::Pos &pos, aoc::Dir dir) {
  aoc::Pos new_pos = pos + aoc::MoveDir(dir);
  bool walkable = aoc::util::IsInMap(height, width, new_pos) && map.at(new_pos.i).at(new_pos.j) == Tile::SAFE;
  return std::make_tuple(walkable, new_pos);
}
i64 Heuristic(const aoc::Pos &pos, const aoc::Pos &goal) {
  i64 distance = (goal.i - pos.i) + (goal.j - pos.j);
  return distance;
}
//void PrintMap(const Map& map, u32 height, u32 width, const Path& path) {
//  for (u32 i = 0; i < height; i++) {
//    for (u32 j = 0; j < height; j++) {
//      if (path.contains(aoc::Pos{i, j})) fmt::print("O"); else fmt::print("{}", aoc::ToUnderlying(map.at(i).at(j)));
//    }
//    fmt::print("\n");
//  }
//}
std::tuple<i64, Path> SolveMap(const Map &map, u32 height, u32 width) {
  aoc::Pos start{0, 0};
  aoc::Pos goal{height - 1, width - 1};
  std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;
  pq.emplace(Heuristic(start, goal), 0, start);
  absl::flat_hash_set<aoc::Pos> visited{};
  absl::flat_hash_map<aoc::Pos, aoc::Pos> predecessor;

  while (!pq.empty()) {
    const auto [h, distance, pos] = pq.top();
    pq.pop();
    if (pos.i == goal.i && pos.j == goal.j) {
      Path path{goal};
      aoc::Pos p = goal;
      while (predecessor.contains(p)) {
        p = predecessor.at(p);
        path.insert(p);
        if (p == start) break;
      }
      return std::make_tuple(distance, path);
    }
    if (visited.contains(pos)) continue;
    visited.insert(pos);
    for (auto d : aoc::kAllDirs) {
      auto [walkable, new_pos] = TryMove(map, height, width, pos, d);
      if (!walkable) continue;
      pq.emplace(Heuristic(new_pos, goal) + distance + 1, distance + 1, new_pos);
      if (!predecessor.contains(new_pos)) predecessor[new_pos] = pos;
    }
  }
  return std::make_tuple(-1, Path{});
}

}  // namespace

namespace fmt {

template<>
struct formatter<Tile> : formatter<char> {
  auto format(Tile tile, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(tile), ctx);
  }
};
template<>
struct formatter<std::vector<Tile>> : formatter<string_view> {
  auto format(std::vector<Tile> row, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("{}", join(row, "")), ctx);
  }
};
template<>
struct formatter<Map> : formatter<string_view> {
  auto format(const Map &map, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("{}", join(map, "\n")), ctx);
  }
};

}  // namespace fmt

template<>
auto advent<2024, 18>::solve() -> Result {
  std::string input = GetInput();
  std::vector<aoc::Pos> bytes = aoc::util::TokenizeInput<aoc::Pos>(input, [](auto line) {
    std::vector<u32> vals;
    CHECK(scn::scan_list_ex(line, vals, scn::list_separator(','))) << "Couldn't parse line '" << line << "'.";
    CHECK(vals.size() == 2) << "Found less than expected values in '" << line << "'.";
    return aoc::Pos(vals.at(1), vals.at(0));
  });

  u32 height = 71;
  u32 width = 71;
  u32 initial_fall = 1024;
  Map map;
  for (u32 i = 0; i < height; i++) map.emplace_back(width, Tile::SAFE);
  for (u32 b = 0; b < initial_fall; b++) {
    const auto &byte = bytes.at(b);
    map[byte.i][byte.j] = Tile::CORRUPTED;
  }

  // Part 1
  auto [part1, path] = SolveMap(map, height, width);

  // Part 2
  std::string part2;
  for (u32 b = initial_fall; b < bytes.size(); b++) {
    const auto &byte = bytes.at(b);
    map[byte.i][byte.j] = Tile::CORRUPTED;
    if (path.contains(byte)) {
      path.clear();
      auto [new_distance, new_path] = SolveMap(map, height, width);
      if (new_distance == -1) {
        part2 = fmt::format("{},{}", byte.j, byte.i);
        break;
      }
      std::swap(path, new_path);
    }
  }

  return aoc::result(part1, part2);
}