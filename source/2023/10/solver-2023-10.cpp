#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>
#include <re2/re2.h>

namespace {

enum Dir { N, S, W, E };
using Pipe = char;
const Pipe NONE = '.';
const Pipe STRT = 'S';
const Pipe VER = '|';  // '\xB3';   // │
const Pipe HOR = '-';  // '\xC4';   // ─
const Pipe SW = '7';   // '\xBF';   // ┐
const Pipe NW = 'J';   // '\xD9';   // ┘
const Pipe NE = 'L';   // '\xC0';   // └
const Pipe SE = 'F';   // '\xDA';   // ┌
char toPipe(char c) {
  switch (c) {
    case '.': return NONE;
    case 'S': return STRT;
    case '|': return VER;
    case '-': return HOR;
    case '7': return SW;
    case 'J': return NW;
    case 'L': return NE;
    case 'F': return SE;
    default: CHECK(false);
  }
}
using Map = std::vector<std::vector<Pipe>>;
absl::flat_hash_map<Dir, Dir> opposite{
    {N, S},
    {S, N},
    {E, W},
    {W, E},
};
absl::flat_hash_map<Pipe, absl::flat_hash_set<Dir>> connectsTo{
    {NONE, {}},
    {STRT, {N, S, W, E}},
    {VER, {N, S}},
    {HOR, {E, W}},
    {SW, {S, W}},
    {NW, {N, W}},
    {NE, {N, E}},
    {SE, {S, E}}
};
const Pipe FREE = ' ';
const Pipe LEFT = 'o';
const Pipe RIGHT = '+';

std::vector<Dir> GetStartDirs(const Map &map, i32 row, i32 col) {
  std::vector<Dir> dirs;
  if (row > 0 && connectsTo.at(map[row - 1][col]).contains(S)) {
    dirs.push_back(N);
  }
  if (row < map.size() - 1 && connectsTo.at(map[row + 1][col]).contains(N)) {
    dirs.push_back(S);
  }
  if (col > 0 && connectsTo.at(map[row][col - 1]).contains(E)) {
    dirs.push_back(W);
  }
  if (col < map[0].size() - 1 && connectsTo.at(map[row][col + 1]).contains(W)) {
    dirs.push_back(E);
  }
  return dirs;
}
std::tuple<i32, i32> MoveDir(i32 row, i32 col, Dir dir) {
  return {row + ((dir == N) ? -1 : (dir == S ? 1 : 0)), col + ((dir == W) ? -1 : (dir == E ? 1 : 0))};
}
std::tuple<i32, i32, Dir> Follow(const Map &map, i32 row, i32 col, Dir dir) {
  auto [new_row, new_col] = MoveDir(row, col, dir);
  for (Dir new_dir : connectsTo[map[new_row][new_col]]) {
    if (new_dir != opposite.at(dir)) {
      return {new_row, new_col, new_dir};
    }
  }
  CHECK(false) << "We were lied about the map!";
}

void FillPaint(Map &map, i32 row, i32 col, char paint) {
  static u32 height = map.size();
  static u32 width = map.at(0).size();
  if (row < 0 || row >= height || col < 0 || col >= width) return;
  if (map[row][col] != FREE) return;
  std::deque<std::tuple<i32, i32>> q;
  q.emplace_back(row, col);
  i32 r;
  i32 c;
  while (!q.empty()) {
    std::tie(r, c) = q.back();
    q.pop_back();
    map[r][c] = paint;
    if (r > 0 && map.at(r - 1).at(c) == FREE) q.emplace_back(r - 1, c);
    if (r < height - 1 && map.at(r + 1).at(c) == FREE) q.emplace_back(r + 1, c);
    if (c > 0 && map.at(r).at(c - 1) == FREE) q.emplace_back(r, c - 1);
    if (c < width - 1 && map.at(r).at(c + 1) == FREE) q.emplace_back(r, c + 1);
  }
}
void PaintPathSides(Map &map, i32 row, i32 col, Dir arrived_by_going, Pipe pipe) {
  switch (arrived_by_going) {
    case N:
      switch (pipe) {
        case VER:FillPaint(map, row, col - 1, LEFT);
          FillPaint(map, row, col + 1, RIGHT);
          break;
        case SW:FillPaint(map, row, col + 1, RIGHT);
          FillPaint(map, row - 1, col, RIGHT);
          break;
        case SE:FillPaint(map, row, col - 1, LEFT);
          FillPaint(map, row - 1, col, LEFT);
          break;
        default: CHECK(false);
      }
      break;
    case S:
      switch (pipe) {
        case VER:FillPaint(map, row, col - 1, RIGHT);
          FillPaint(map, row, col + 1, LEFT);
          break;
        case NW:FillPaint(map, row, col + 1, LEFT);
          FillPaint(map, row + 1, col, LEFT);
          break;
        case NE:FillPaint(map, row, col - 1, RIGHT);
          FillPaint(map, row + 1, col, RIGHT);
          break;
        default: CHECK(false);
      }
      break;
    case E:
      switch (pipe) {
        case HOR:FillPaint(map, row - 1, col, LEFT);
          FillPaint(map, row + 1, col, RIGHT);
          break;
        case NW:FillPaint(map, row, col + 1, RIGHT);
          FillPaint(map, row + 1, col, RIGHT);
          break;
        case SW:FillPaint(map, row, col + 1, LEFT);
          FillPaint(map, row - 1, col, LEFT);
          break;
        default: CHECK(false);
      }
      break;
    case W:
      switch (pipe) {
        case HOR:FillPaint(map, row - 1, col, RIGHT);
          FillPaint(map, row + 1, col, LEFT);
          break;
        case NE:FillPaint(map, row, col - 1, LEFT);
          FillPaint(map, row + 1, col, LEFT);
          break;
        case SE:FillPaint(map, row, col - 1, RIGHT);
          FillPaint(map, row - 1, col, RIGHT);
          break;
        default: CHECK(false);
      }
      break;
    default: CHECK(false);
  }
}

}  // namespace

namespace fmt {

template<>
struct fmt::formatter<Map> : formatter<string_view> {
  auto format(const Map &m, format_context &ctx) const {
    std::vector<std::string> rows;
    absl::c_transform(m, std::back_inserter(rows), [](std::vector<Pipe> row) {
      return fmt::format("{}", string_view{row.data(), row.size()});
    });
    return fmt::format_to(ctx.out(), "{}", fmt::join(rows, "\n"));
  }
};
template<>
struct fmt::formatter<Dir> : formatter<string_view> {
  auto format(Dir dir, format_context &ctx) const {
    switch (dir) {
      case N: return formatter<string_view>::format("N", ctx);
      case S: return formatter<string_view>::format("S", ctx);
      case E: return formatter<string_view>::format("E", ctx);
      case W: return formatter<string_view>::format("W", ctx);
    }
  }
};

}  // namespace fmt

template<>
auto advent<2023, 10>::solve() -> Result {
  std::string input = GetInput(false);
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Map map;
  i32 start_row;
  i32 start_col;
  for (i32 row = 0; row < height; row++) {
    map.emplace_back(width);
    for (i32 col = 0; col < width; col++) {
      char c = lines.at(row).at(col);
      map[row][col] = c;
      if (c == STRT) {
        start_row = row;
        start_col = col;
      }
    }
  }

  // Prep for part 2
  Map paintMap{};
  for (i32 row = 0; row < height; row++) {
    paintMap.emplace_back();
    for (i32 col = 0; col < width; col++) {
      paintMap.back().push_back(FREE);
    }
  }

  // Part 1
  auto start_dirs = GetStartDirs(map, start_row, start_col);
  CHECK(start_dirs.size() == 2);
  i32 row{start_row};
  i32 col{start_col};
  Dir dir = start_dirs.at(0);
  std::vector<Dir> path{dir};
  while (true) {
    paintMap[row][col] = map[row][col];
    std::tie(row, col, dir) = Follow(map, row, col, dir);
    if (map.at(row).at(col) == STRT) break;
    path.push_back(dir);
  }
  u64 part1 = path.size() / 2;

  // Part 2
  row = start_row;
  col = start_col;
  // Paint sides of Start.
  Pipe actual_start_pipe = NONE;
  for (auto [k, v] : connectsTo) {
    if (v.size() == 2 && v.contains(start_dirs.at(0)) && v.contains(start_dirs.at(1))) {
      actual_start_pipe = k;
    }
  }
  CHECK(actual_start_pipe != NONE) << "Unable to identify actual start pipe.";
  PaintPathSides(paintMap, row, col, path.back(), actual_start_pipe);
  for (int d = 0; d < path.size() - 1; d++) {
    std::tie(row, col) = MoveDir(row, col, path.at(d));
    PaintPathSides(paintMap, row, col, path.at(d), map.at(row).at(col));
  }

  // Identify inside and outside.
  char inside = '?';
  // Try top and bottom rows
  for (u32 c = 0; c < width; c++) {
    for (u32 r = 0; r < height; r += height - 1) {
      char paint = paintMap.at(r).at(c);
      if (paint == LEFT) {
        inside = RIGHT;
        break;
      }
      if (paint == RIGHT) {
        inside = LEFT;
        break;
      }
    }
  }
  // Try left and right sides
  if (inside == '?') {
    for (u32 c = 0; c < width; c += width - 1) {
      for (u32 r = 0; r < height; r++) {
        char paint = paintMap.at(r).at(c);
        if (paint == LEFT) {
          inside = RIGHT;
          break;
        }
        if (paint == RIGHT) {
          inside = LEFT;
          break;
        }
      }
    }
  }
  CHECK(inside != '?') << "Couldn't find the inside color!";
  u64 part2 = absl::c_accumulate(paintMap, 0, [inside](u32 prev, const std::vector<char> &row) {
    return prev + absl::c_count_if(row, [inside](char c) { return c == inside; });
  });

  return aoc::result(part1, part2);
}