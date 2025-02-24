#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>

namespace {

enum class Tile : char {
  NORMAL = '.',
  FOREST = '#',
  ICE_N = '^',
  ICE_E = '>',
  ICE_S = 'v',
  ICE_W = '<',
};
using Map = std::vector<std::vector<Tile>>;
using CostAndPos = std::tuple<i64, aoc::Pos>;
using NodeId = aoc::Pos;
struct Edge {
  NodeId a;
  NodeId b;
  i32 len;
  Edge(const NodeId &_a, const NodeId &_b, i32 _len) : len(_len) {
    if (_a < _b) {
      a = _a;
      b = _b;
    } else {
      a = _b;
      b = _a;
    }
  }
  [[nodiscard]] bool HasId(const NodeId &x) const { return a == x || b == x; }
  [[nodiscard]] NodeId Other(const NodeId &x) const {
    if (!HasId(x)) CHECK(false) << fmt::format("Asked for counterpart of {} in {}-{}.", x, a, b);
    return x == a ? b : a;
  }
  bool operator<(const Edge &o) const {
    return a < o.a || (a == o.a && b < o.b) || (a == o.a && b == o.b && len < o.len);
  }
  bool operator==(const Edge &o) const { return a == o.a && b == o.b && len == o.len; }
  template<typename H>
  friend H AbslHashValue(H h, const Edge &e) {
    return H::combine(std::move(h), e.a, e.b);
  }
};

using Graph = absl::btree_map<NodeId, absl::flat_hash_set<Edge>>;

Tile FromUnderlying(char c) {
  switch (c) {
    case '.': return Tile::NORMAL;
    case '#': return Tile::FOREST;
    case '^': return Tile::ICE_N;
    case '>': return Tile::ICE_E;
    case 'v': return Tile::ICE_S;
    case '<': return Tile::ICE_W;
    default: CHECK(false) << "Unrecognized map tile '" << c << "'.";
  }
}

}  // namespace

namespace fmt {

template<>
struct formatter<Tile> : formatter<char> {
  auto format(Tile tile, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(tile), ctx);
  }
};

}  // namespace fmt

namespace {

//void PrintMap(const Map &map) {
//  for (const auto &row : map) {
//    fmt::print("{}\n", fmt::join(row, ""));
//  }
//}

std::tuple<bool, aoc::Pos> TryMove(const Map &map, const aoc::Pos &from, aoc::Dir dir) {
  Tile cur_tile = map[from.i][from.j];
  CHECK(cur_tile != Tile::FOREST) << "Don't go in the forest!";
  if (cur_tile != Tile::NORMAL && aoc::ToUnderlying(cur_tile) != aoc::ToUnderlying(dir)) {
    // Only go down in slopes.
    return {false, from};
  }
  auto new_pos = from + aoc::MoveDir(dir);
  if (new_pos.i < 0 || new_pos.i >= map.size()) return {false, from};
  Tile new_tile = map[new_pos.i][new_pos.j];
  if (new_tile == Tile::FOREST) {
    // No entering forests.
    return {false, new_pos};
  }
  if (new_tile == Tile::NORMAL) {
    return {true, new_pos};
  }

  if (aoc::ToUnderlying(new_tile) == aoc::ToUnderlying(dir)) {
    // Can always follow the slope downwards.
    return {true, new_pos};
  }
  if (aoc::ToUnderlying(new_tile) == aoc::ToUnderlying(aoc::OppositeDir(dir))) {
    // Can't climb a slope.
    return {false, new_pos};
  }
  CHECK(false) << fmt::format("Travelling {} parallel to a slop at [{}], slope: {}", dir, new_pos, new_tile);
}

u64 BellManFord(const Map &map, const aoc::Pos &start, const aoc::Pos &goal) {
  std::priority_queue<CostAndPos, std::vector<CostAndPos>, std::greater<>> pq;
  absl::btree_map<aoc::Pos, i64> distance;
  absl::btree_map<aoc::Pos, aoc::Pos> predecessor;
  distance[start] = 0;
  pq.emplace(0, start);
  while (!pq.empty()) {
    const auto [dist, pos] = pq.top();
    pq.pop();
    if (pos == goal) continue;
    for (auto d : aoc::kAllDirs) {
      auto [possible, new_pos] = TryMove(map, pos, d);
      if (!possible) continue;
      if (predecessor.contains(pos) && new_pos == predecessor.at(pos)) continue;
      if (distance.contains(new_pos) && distance.at(new_pos) <= distance.at(pos) - 1) continue;
      distance[new_pos] = dist - 1;
      predecessor[new_pos] = pos;
      pq.emplace(distance.at(new_pos), new_pos);
    }
  }
  return -distance.at(goal);
}

std::tuple<bool, std::vector<aoc::Dir>> isNode(const Map &map, const aoc::Pos &pos) {
  if (map.at(pos.i).at(pos.j) == Tile::FOREST) return {false, {}};
  std::vector<aoc::Dir> exits;
  i32 paths{0};
  for (auto dir : aoc::kAllDirs) {
    auto delta = aoc::MoveDir(dir);
    auto new_tile = map.at(pos.i + delta.i).at(pos.j + delta.j);
    if (new_tile == Tile::FOREST) continue;
    paths++;
    if (new_tile == Tile::NORMAL) continue;
    if (aoc::ToUnderlying(new_tile) == aoc::ToUnderlying(dir)) {
      exits.push_back(dir);
    }
  }
  return {paths > 2, exits};
}

std::tuple<aoc::Pos, i32> FindNextNode(const Map &map,
                                       const aoc::Pos &from,
                                       const absl::btree_set<NodeId> &nodes,
                                       aoc::Dir dir) {
  i32 steps{1};
  aoc::Pos pos{from + aoc::MoveDir(dir)};
  auto prev_dir = dir;
  while (!nodes.contains(pos)) {
    for (auto d : aoc::kAllDirs) {
      if (d == aoc::OppositeDir(prev_dir)) continue;
      auto new_pos = pos + aoc::MoveDir(d);
      if (map.at(new_pos.i).at(new_pos.j) == Tile::FOREST) continue;
      steps++;
      prev_dir = d;
      pos = new_pos;
      break;
    }
  }
  return {pos, steps};
}

Graph BuildUDAG(const Map &map, const aoc::Pos &start, const aoc::Pos &goal) {
  u32 height = map.size();
  u32 width = map.at(0).size();
  absl::btree_map<NodeId, std::vector<aoc::Dir>> node_exits;
  absl::btree_set<NodeId> node_ids;
  node_exits[start] = {aoc::Dir::S};
  node_ids.insert(start);
  node_ids.insert(goal);
  for (i32 i = 1; i < height - 1; i++) {
    for (i32 j = 1; j < width - 1; j++) {
      aoc::Pos pos{i, j};
      auto [is_node, exits] = isNode(map, pos);
      if (!is_node) continue;
      node_ids.insert(pos);
      node_exits[pos] = exits;
    }
  }
  Graph graph{};
  absl::flat_hash_set<Edge> edges{};
  for (const auto &[node, exits] : node_exits) {
    for (auto exit : exits) {
      auto [target, steps] = FindNextNode(map, {node}, node_ids, exit);
      Edge edge{node, target, steps};
      CHECK(!edges.contains(edge)) << fmt::format("Re-calculated link ID {}-{}.", edge.a, edge.b);
      edges.insert(edge);
    }
  }
  for (const auto &edge : edges) {
    graph[edge.a].insert(edge);
    graph[edge.b].insert(edge);
  }
  return graph;
}

u64 LongestPath(const Graph &graph, const NodeId &start, const NodeId &goal) {
  using State = std::tuple<std::vector<NodeId>, u64>;
  std::deque<State> stack;
  stack.push_back({{start}, 0});
  u64 longest{0};

  while (!stack.empty()) {
    auto [path, length] = stack.back();
    stack.pop_back();
    const NodeId& cur = path.back();
    if (cur == goal) {
      if (length > longest) longest = length;
      continue;
    }
    for (const auto &edge : graph.at(cur)) {
      const NodeId& other = edge.Other(cur);
      if (std::find(path.begin(), path.end(), other) != path.end()) {
        continue;
      }
      std::vector<NodeId> new_path{path.begin(), path.end()};
      new_path.push_back(other);
      State new_state{new_path, length + edge.len};
      stack.push_back(new_state);
    }
  }
  return longest;
}

}

template<>
auto advent<2023, 23>::solve() -> Result {
  std::string input = GetInput(false);
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Map map;
  for (i32 i = 0; i < height; i++) {
    map.emplace_back();
    for (i32 j = 0; j < width; j++) {
      map.back().push_back(FromUnderlying(lines.at(i).at(j)));
    }
  }

  // Part 1
  aoc::Pos start{0, 1};
  aoc::Pos goal{height - 1, width - 2};
  u64 part1 = BellManFord(map, start, goal);

  // Part 2
  auto graph = BuildUDAG(map, start, goal);
  u64 part2 = LongestPath(graph, start, goal);

  return aoc::result(part1, part2);
}