#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

enum class Tile : char {
  EMPTY = '.',
  WALL = '#',
};
using Row = std::vector<Tile>;
using Map = std::vector<Row>;
using PosDir = std::tuple<aoc::Pos, aoc::Dir>;
using ScorePosDir = std::tuple<i64, aoc::Pos, aoc::Dir>;
using State = std::priority_queue<ScorePosDir, std::vector<ScorePosDir>, std::greater<>>;

std::tuple<bool, aoc::Pos> TryMove(const Map &map, const aoc::Pos &from, aoc::Dir dir) {
  auto new_pos = from + aoc::MoveDir(dir);
  return {map.at(new_pos.i).at(new_pos.j) == Tile::EMPTY, new_pos};
}

std::tuple<u64, u64> SolveMap(const Map &map) {
  u64 height = map.size(), width = map.at(0).size();
  aoc::Pos start{static_cast<i64>(height) - 2, 1}, end{1, static_cast<i64>(width) - 2};
  aoc::Dir start_dir{aoc::Dir::E};
  State pq;
  pq.emplace(0, start, start_dir);
  absl::flat_hash_map<PosDir, absl::flat_hash_set<PosDir>> predecessor;
  absl::btree_map<PosDir, u64> scores;
  scores[{start, start_dir}] = 0;
  absl::flat_hash_map<PosDir, i64> visited{};
  i64 end_score = std::numeric_limits<i64>::max();
  while (!pq.empty()) {
    const auto [score, pos, dir] = pq.top();
    pq.pop();
    if (pos == end) continue;
    PosDir pos_dir{pos, dir};
    for (auto d : aoc::kAllDirs) {
      if (d == aoc::OppositeDir(dir)) continue;
      auto [can_move, new_pos] = TryMove(map, pos, d);
      if (!can_move) continue;
      u64 extra_score = (dir != d) ? 1001 : 1;
      u64 new_score = score + extra_score;
      PosDir new_pos_dir{new_pos, d};
      if (scores.contains(new_pos_dir) && scores.at(new_pos_dir) < new_score) continue;
      if (!scores.contains(new_pos_dir) || scores.at(new_pos_dir) > new_score) {
        scores[new_pos_dir] = new_score;
        predecessor[new_pos_dir].clear();
      }
      predecessor[new_pos_dir].insert(pos_dir);
      pq.emplace(new_score, new_pos, d);
    }
  }
  PosDir end_n{end, aoc::Dir::N};
  PosDir end_e{end, aoc::Dir::E};
  u64 best_score = std::min(scores.at(end_e), scores.at(end_n));
  std::deque<PosDir> q;
  if (scores.at(end_n) == best_score) q.push_back(end_n);
  if (scores.at(end_e) == best_score) q.push_back(end_e);
  absl::flat_hash_set<aoc::Pos> best_seats;
  while (!q.empty()) {
    auto pos_dir = q.front();
    q.pop_front();
    auto pos = std::get<0>(pos_dir);
    best_seats.insert(pos);
    if (pos == start) continue;
    for (const auto &pred : predecessor.at(pos_dir)) {
      q.push_back(pred);
    }
  }

  return {best_score, best_seats.size()};
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
auto advent<2024, 16>::solve() -> Result {
  Map map = aoc::util::TokenizeInput<Row>(input, [](auto line) {
    Row row;
    absl::c_transform(line, std::back_inserter(row), [](char ch) { return (ch == '#') ? Tile::WALL : Tile::EMPTY; });
    return row;
  });

  // Part 1 & Part 2
  auto [part1, part2] = SolveMap(map);

  return aoc::result(part1, part2);
}