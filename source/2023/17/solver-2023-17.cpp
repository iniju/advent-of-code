#include <aoc.hpp>

#include <absl/container/flat_hash_set.h>
#include <absl/hash/hash.h>
#include <Eigen/Dense>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace {
enum class Dir : char {
  N = '^',
  E = '>',
  S = 'v',
  W = '<',
};
std::array<Dir, 4> all_dirs{Dir::N, Dir::E, Dir::S, Dir::W};
std::tuple<i32, i32> MoveDir(Dir dir) {
  static const std::array<std::tuple<i32, i32>, 4> moves{{
                                                             {-1, 0}, {0, 1}, {1, 0}, {0, -1}
                                                         }};
  switch (dir) {
    case Dir::N: return moves[0];
    case Dir::E: return moves[1];
    case Dir::S: return moves[2];
    case Dir::W: return moves[3];
  }

}
Dir OppositeDir(Dir dir) {
  switch (dir) {
    case Dir::N: return Dir::S;
    case Dir::E: return Dir::W;
    case Dir::S: return Dir::N;
    case Dir::W: return Dir::E;
  }
}
using Map = Eigen::MatrixXi;
//Eigen::IOFormat MapFormat(Eigen::StreamPrecision, 0, "", "\n");
struct Node {
  i32 i;
  i32 j;
  u64 heat_loss;
  Dir last_dir;
  i32 dir_length;
  bool operator>(const Node &r) const {
    return heat_loss > r.heat_loss;
  }
  bool operator==(const Node &r) const {
    return i == r.i && j == r.j && heat_loss == r.heat_loss && last_dir == r.last_dir && dir_length == r.dir_length;
  }
};
using VisitedNode = std::tuple<i32, i32, Dir, i32>;
template<typename H>
H AbslHashValue(H h, const Node &n) {
  return H::combine(std::move(h), n.i, n.j, n.heat_loss, n.last_dir, n.dir_length);
}
i32 MoveConstraint1(const Node &node, Dir dir) {
  return node.last_dir != dir || node.dir_length < 3;
}
i32 MoveConstraint2(const Node &node, Dir dir) {
  if (node.dir_length == 0) {
    return 4;
  }
  if (node.last_dir != dir && node.dir_length >= 4) {
    return 4;
  }
  if (node.last_dir == dir && node.dir_length < 10) {
    return 1;
  }
  return 0;
}
u64 FindBestPath(const Map &map, std::function<i32(const Node &, Dir)> constraint) {
  u32 height = map.rows();
  u32 width = map.cols();
  u32 goal_i = height - 1;
  u32 goal_j = width - 1;
  std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;
  pq.push({0, 0, 0, Dir::S, 0});
  absl::flat_hash_set<VisitedNode> visited{};

  while (!pq.empty()) {
    const auto cur = pq.top();
    pq.pop();
    if (cur.i == goal_i && cur.j == goal_j) {
      return cur.heat_loss;
    }
    VisitedNode visited_node{cur.i, cur.j, cur.last_dir, cur.dir_length};
    if (visited.count(visited_node)) {
      // Been here before.
      continue;
    }
    visited.insert(visited_node);
    for (auto d : all_dirs) {
      if (d == OppositeDir(cur.last_dir)) {
        // Can't go backwards.
        continue;
      }
      i32 try_move = constraint(cur, d);
      if (try_move == 0) {
        // Can't move this way.
        continue;
      }
      auto [di, dj] = MoveDir(d);
      i32 ni = cur.i + di * try_move;
      i32 nj = cur.j + dj * try_move;
      if (ni < 0 || ni >= height || nj < 0 || nj >= width) {
        // Out of frame.
        continue;
      }
      u64 new_heat_loss = cur.heat_loss;
      for (i32 m = 1; m <= try_move; m++) {
        new_heat_loss += map(cur.i + di * m, cur.j + dj * m);
      }

      pq.push({ni, nj, new_heat_loss, d, d == cur.last_dir ? cur.dir_length + try_move : try_move});
    }
  }
  return -1;
}

}  // namespace

namespace fmt {

template<>
struct formatter<Eigen::WithFormat<Map>> : ostream_formatter {};

}  // namespace fmt

template<>
auto advent<2023, 17>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Map map;
  map.resize(height, width);
  for (i32 i = 0; i < height; i++) {
    for (i32 j = 0; j < width; j++) {
      map(i, j) = lines.at(i).at(j) - '0';
    }
  }


  // Part 1
  u64 part1 = FindBestPath(map, MoveConstraint1);

  // Part 2
  u64 part2 = FindBestPath(map, MoveConstraint2);

  return aoc::result(part1, part2);
}