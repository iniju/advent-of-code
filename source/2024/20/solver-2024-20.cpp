#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

enum class Tile : char {
  EMPTY = '.',
  WALL = '#',
  START = 'S',
  END = 'E',
};

using Row = std::vector<Tile>;
using Map = std::vector<Row>;
using Path = std::vector<aoc::Pos>;

std::tuple<bool, aoc::Pos> TryMove(const Map &map,
                                   u32 height,
                                   u32 width,
                                   const aoc::Pos &from,
                                   aoc::Dir dir) {
  auto new_pos = from + aoc::MoveDir(dir);
  if (aoc::util::IsOutOfMap(height, width, new_pos)) return {false, new_pos};
  auto tile = map[new_pos.i][new_pos.j];
  if (tile == Tile::WALL) return {false, new_pos};
  return {true, new_pos};
}

Path SolveNoCheat(const Map &map, u32 height, u32 width, aoc::Pos &start, aoc::Pos &end) {
  std::vector<aoc::Pos> result{start};
  aoc::Pos pos, prev;
  pos = start;
  bool started = false;
  while (pos != end) {
    for (aoc::Dir d : aoc::kAllDirs) {
      auto [can_move, new_pos] = TryMove(map, height, width, pos, d);
      if (!can_move) continue;
      if (started && new_pos == prev) continue;
      started = true;
      result.push_back(new_pos);
      prev = pos;
      pos = new_pos;
    }
  }
  return result;
}

absl::btree_map<u32, u32> SolveWithCheats(const Map &map,
                                          u32 height,
                                          u32 width,
                                          const Path &no_cheat_path,
                                          i32 budget) {
  absl::btree_map<u32, u32> result{};
  const auto &end = no_cheat_path.back();
  u32 no_cheat_dist = no_cheat_path.size() - 1;
  absl::flat_hash_set<std::pair<aoc::Pos, aoc::Pos>>
      already_counted;
  absl::flat_hash_map<aoc::Pos, u32> distance_from_end;
  for (u32 p = 0; p <= no_cheat_dist; p++) {
    distance_from_end[no_cheat_path.at(p)] = no_cheat_dist - p;
  }
  for (const auto &cheat_at : no_cheat_path) {
    if (cheat_at == end) break;
    for (i32 di = -budget; di <= budget; di++) {
      i32 j_budget = budget - abs(di);
      for (i32 dj = -j_budget; dj <= j_budget; dj++) {
        if (di == 0 && dj == 0) continue;
        aoc::Pos new_pos{cheat_at.i + di, cheat_at.j + dj};
        if (aoc::util::IsOutOfMap(height, width, new_pos)) continue;
        if (map.at(new_pos.i).at(new_pos.j) == Tile::WALL) continue;
        std::pair<aoc::Pos, aoc::Pos> pos_pair = std::make_pair(cheat_at, new_pos);
        i32 cheat_steps = abs(di) + abs(dj);
        if (already_counted.contains(pos_pair)) continue;
        if (distance_from_end.at(cheat_at) > distance_from_end.at(new_pos) + cheat_steps) {
          u32 savings = distance_from_end.at(cheat_at) - distance_from_end.at(new_pos) - cheat_steps;
          result[savings]++;
          already_counted.insert(pos_pair);
        }
      }
    }
  }
  return result;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 20>::solve() -> Result {
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Map map;
  aoc::Pos start, end;
  for (u32 i = 0; i < height; i++) {
    map.emplace_back();
    for (u32 j = 0; j < width; j++) {
      char ch = lines.at(i).at(j);
      switch (ch) {
        case '.': map[i].push_back(Tile::EMPTY);
          break;
        case '#': map[i].push_back(Tile::WALL);
          break;
        case 'S': map[i].push_back(Tile::START);
          start.i = i;
          start.j = j;
          break;
        case 'E': map[i].push_back(Tile::END);
          end.i = i;
          end.j = j;
          break;
        default: CHECK(false) << "Unexpected character in map '" << ch << "'.";
      }
    }
  }

  // Part 1
  auto no_cheat_path = SolveNoCheat(map, height, width, start, end);
  auto cheat_saves_1 = SolveWithCheats(map, height, width, no_cheat_path, 2);
  u64 part1 = 0;
  for (auto it = cheat_saves_1.lower_bound(100); it != cheat_saves_1.end(); it++) {
    part1 += it->second;
  }

  // Part 2
  auto cheat_saves_2 = SolveWithCheats(map, height, width, no_cheat_path, 20);
  u64 part2 = 0;
  for (auto it = cheat_saves_2.lower_bound(100); it != cheat_saves_2.end(); it++) {
    part2 += it->second;
  }

  return aoc::result(part1, part2);
}