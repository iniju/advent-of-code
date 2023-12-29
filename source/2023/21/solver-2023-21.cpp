#include <aoc.hpp>

#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace {

using Map = Eigen::MatrixXi;
//Eigen::IOFormat MapFormat(Eigen::StreamPrecision, 0, "", "\n");

using Chunks = absl::flat_hash_set<std::pair<aoc::Pos, aoc::Pos>>;
u64 Walk(const Map &map, const aoc::Pos &start, u64 budget) {
  u32 height = map.rows();
  u32 width = map.cols();
  u64 reached{0};
  absl::flat_hash_set<aoc::Pos> visited{};
  visited.insert(start);
  std::deque<std::tuple<aoc::Pos, u64>> q;
  q.emplace_back(start, 0);
  while (!q.empty()) {
    auto [pos, steps] = q.front();
    q.pop_front();
    if (steps % 2 == budget % 2) {
      reached++;
    }
    if (steps == budget) continue;
    steps++;
    for (auto d : aoc::kAllDirs) {
      auto new_pos = pos + aoc::MoveDir(d);
      if (new_pos.i < 0 || new_pos.i >= height || new_pos.j < 0 || new_pos.j >= width) {
        continue;
      }
      if (map(new_pos.i, new_pos.j) == 1) continue;
      if (visited.contains(new_pos)) continue;
      visited.insert(new_pos);
      q.emplace_back(new_pos, steps);
    }
  }
  return reached;
}

}  // namespace

namespace fmt {

template<>
struct formatter<Eigen::WithFormat<Map>> : ostream_formatter {};

}  // namespace fmt

template<>
auto advent<2023, 21>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Map map;
  map.resize(height, width);
  map.setZero();
  aoc::Pos start{0, 0};
  for (i32 i = 0; i < height; i++) {
    for (i32 j = 0; j < width; j++) {
      char c = lines.at(i).at(j);
      if (c == '#') {
        map(i, j) = 1;
      } else if (c == 'S') {
        start.i = i;
        start.j = j;
      }
    }
  }

  // Part 1
  auto part1 = Walk(map, start, 64);

  // Part 2
  // Interesting observations:
  // - Edges of the grid are clear.
  // - clear column and clear row meeting at S.
  // - This means the shortest path to any edge [i, j] in any neighboring map chunk is
  //   e.g. for chunk to the north at [130, j]:
  //   A. Move north 66 tiles, barely crossing to next chunk.
  //   B. Move to j column.
  // - Steps = 26,501,365 = 202,300 * 131 + 65
  // - This means that we can reach 202,300 map chunks to N, S, E and W and barely reach the N, S, E and W edge of each
  //   respectively.
  // - The explored area roughly covers a square (rotated 45 degrees) with diagonal 2 * 202,300 chunks.
  // - Assuming the partially covered chunks fill other partially covered chunks perfectly, this amounts to:
  //   2 * 202,300^2 chunks.
  // - Our number of steps is odd, this means that in the initial chunk we reach every [i, j] where i + j is odd with
  //   zero-based indexing.
  // - Because the distance from S to edge is an odd number, it means when we reach the next chunk from the starting, we
  //   would be flip the reaching tiles to where i+j is even.
  // - If the initial chunk is [0, 0], generalizing for chunk [ci, cj], we reach tiles [i, j] where ci + cj + i + j is
  //   odd.
  // - Let odd chunks O be the ones where we reach every [i, j] where i + j is odd, like the starting one.
  // - Let even chunks E be the ones where we reach every [i, j] when i + j is even, like the ones bordering the
  //   starting one.
  // - Then pattern of E/O chunks looks like this.
  //                                  +---+---+---+
  //                                  | . | P | . |
  //                              +---+---+---+---+---+
  //                              | . | F | E | F | . |
  //                          +---+---+---+---+---+---+---+
  //                          | . | F | E | O | E | F | . |
  //                      +---+---+---+---+---+---+---+---+---+
  //                      | . | F | E | O | E | O | E | F | . |
  //                  +---+---+---+---+---+---+---+---+---+---+---+
  //                  | . | F | E | O | E | O | E | O | E | F | . |
  //              +---+---+---+---+---+---+---+---+---+---+---+---+---+
  //              | . | F | E | O | E | O | E | O | E | O | E | F | . |
  //          +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //          | . | F | E | O | E | O | E | O | E | O | E | O | E | F | . |
  //      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //      | . | F | E | O | E | O | E | O | E | O | E | O | E | O | E | F | . |
  //      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //      | P | E | O | E | O | E | O | E | O | E | O | E | O | E | O | E | P |
  //      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //      | . | F | E | O | E | O | E | O | E | O | E | O | E | O | E | F | . |
  //      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //          | . | F | E | O | E | O | E | O | E | O | E | O | E | F | . |
  //          +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //              | . | F | E | O | E | O | E | O | E | O | E | F | . |
  //              +---+---+---+---+---+---+---+---+---+---+---+---+---+
  //                  | . | F | E | O | E | O | E | O | E | F | . |
  //                  +---+---+---+---+---+---+---+---+---+---+---+
  //                      | . | F | E | O | E | O | E | F | . |
  //                      +---+---+---+---+---+---+---+---+---+
  //                          | . | F | E | O | E | F | . |
  //                          +---+---+---+---+---+---+---+
  //                              | . | F | E | F | . |
  //                              +---+---+---+---+---+
  //                                  | . | P | . |
  //                                  +---+---+---+
  // - As can be seen above, the area that can be covered is a diamond shape with the following characteristics:
  // - D^2 chunks are type even and fully covered.
  // - (D-1)^2 chunks are type odd and fully covered.
  // - There are 4*D chunks (even type) that are partially covered from one corner only (D for each of the corners).
  // - There are 4*(D-1) chunks (odd type) almost fully covered, only missing one corner (D-1 for each corner).
  // - There are 4 odd type chunks that are covered from one edge entirely and reaching just up to the other edge.

  u64 part2{0};
  u64 d = 26501365ll / 131ll;
  // Full ones
  auto fullOdd = Walk(map, {64, 64}, 141);
  auto fullEven = Walk(map, {64, 64}, 142);
  // Almost empty (even)
  auto cornerSW = Walk(map, {130, 0}, 64);
  auto cornerNW = Walk(map, {0, 0}, 64);
  auto cornerSE = Walk(map, {130, 130}, 64);
  auto cornerNE = Walk(map, {0, 130}, 64);
  // Almost full (odd)
  auto fullSW = Walk(map, {130, 0}, 195);
  auto fullNW = Walk(map, {0, 0}, 195);
  auto fullSE = Walk(map, {130, 130}, 195);
  auto fullNE = Walk(map, {0, 130}, 195);
  // Pointy ones (odd)
  auto PointyEast = Walk(map, {65, 0}, 130);
  auto PointyWest = Walk(map, {65, 130}, 130);
  auto PointyNorth = Walk(map, {130, 65}, 130);
  auto PointySouth = Walk(map, {0, 65}, 130);

  part2 += d * d * fullEven;
  part2 += (d - 1) * (d - 1) * fullOdd;
  part2 += d * (cornerSW + cornerNW + cornerNE + cornerSE);
  part2 += (d - 1) * (fullSW + fullNW + fullNE + fullSE);
  part2 += PointyNorth + PointyEast + PointySouth + PointyWest;

  return aoc::result(part1, part2);
}