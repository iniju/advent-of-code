#include <aoc.hpp>

#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>

namespace {

enum class Tile : char {
  EMPTY = '.',
  MIR_NE = '/',
  MIR_NW = '\\',
  SPL_VRT = '|',
  SPL_HOR = '-',
};
enum class Dir : char {
  NORTH = '^',
  EAST = '>',
  SOUTH = 'v',
  WEST = '<',
};
using Pos = std::tuple<i32, i32>;
struct Beam {
  Pos pos;
  Dir dir;
};
using Map = std::vector<std::vector<Tile>>;
using BeamMap = std::vector<std::vector<absl::flat_hash_set<Dir>>>;
using Beams = std::deque<Beam>;

Dir OppositeOf(Dir dir) {
  switch (dir) {
    case Dir::NORTH: return Dir::SOUTH;
    case Dir::EAST: return Dir::WEST;
    case Dir::SOUTH: return Dir::NORTH;
    case Dir::WEST: return Dir::EAST;
  }
}

Tile ParseTile(char c) {
  switch (c) {
    case '.': return Tile::EMPTY;
    case '/': return Tile::MIR_NE;
    case '\\': return Tile::MIR_NW;
    case '|': return Tile::SPL_VRT;
    case '-': return Tile::SPL_HOR;
    default: CHECK(false) << "Unexpected tile '" << c << "'.";
  }
}
bool IsMirror(Tile tile) {
  return tile == Tile::MIR_NW || tile == Tile::MIR_NE;
}
Pos MoveBeam(const Beam &beam) {
  auto [i, j] = beam.pos;
  switch (beam.dir) {
    case Dir::NORTH: return {i - 1, j};
    case Dir::EAST: return {i, j + 1};
    case Dir::SOUTH: return {i + 1, j};
    case Dir::WEST: return {i, j - 1};
  }
}
std::vector<Beam> TileEffect(Tile tile, const Beam &beam) {
  switch (tile) {
    case Tile::EMPTY: return {beam};
    case Tile::MIR_NE:
      switch (beam.dir) {
        case Dir::NORTH: return {{beam.pos, Dir::EAST}};
        case Dir::EAST: return {{beam.pos, Dir::NORTH}};
        case Dir::SOUTH: return {{beam.pos, Dir::WEST}};
        case Dir::WEST: return {{beam.pos, Dir::SOUTH}};
      }
    case Tile::MIR_NW:
      switch (beam.dir) {
        case Dir::NORTH: return {{beam.pos, Dir::WEST}};
        case Dir::EAST: return {{beam.pos, Dir::SOUTH}};
        case Dir::SOUTH: return {{beam.pos, Dir::EAST}};
        case Dir::WEST: return {{beam.pos, Dir::NORTH}};
      }
    case Tile::SPL_VRT:if (beam.dir == Dir::NORTH || beam.dir == Dir::SOUTH) return {beam};
      return {{beam.pos, Dir::NORTH}, {beam.pos, Dir::SOUTH}};
    case Tile::SPL_HOR:if (beam.dir == Dir::EAST || beam.dir == Dir::WEST) return {beam};
      return {{beam.pos, Dir::EAST}, {beam.pos, Dir::WEST}};
  }
}
u64 CountEnergy(const BeamMap &beam_map) {
  u64 result{0};
  for (i32 i = 0; i < beam_map.size(); i++) {
    for (i32 j = 0; j < beam_map[0].size(); j++) {
      if (!beam_map.at(i).at(j).empty()) result++;
    }
  }
  return result;
}
u64 Energize(const Map &map, const Beam &initial_beam) {
  u32 height = map.size();
  u32 width = map.at(0).size();
  BeamMap beam_map{};
  for (i32 i = 0; i < height; i++) {
    beam_map.emplace_back(width);
  }
  Beams beams{initial_beam};
  // absl::flat_hash_set<Pos> &latest;
  while (!beams.empty()) {
    auto beam = beams.front();
    beams.pop_front();
    auto [i, j] = MoveBeam(beam);
    if (i < 0 || i >= height || j < 0 || j >= width) {
      // Beam outside the frame.
      continue;
    }
    if (beam_map.at(i).at(j).contains(beam.dir)) {
      // Beam visited here before.
      continue;
    }
    if (beam_map.at(i).at(j).contains(OppositeOf(beam.dir)) && !IsMirror(map.at(i).at(j))) {
      // Beam visited here before in opposite direction.
      continue;
    }
    beam_map[i][j].insert(beam.dir);

    for (const auto &new_beam : TileEffect(map.at(i).at(j), {{i, j}, beam.dir})) {
      beams.push_front(new_beam);
    }
    // latest.insert({i, j});
    if (map.at(i).at(j) == Tile::EMPTY ||
        (map.at(i).at(j) == Tile::SPL_VRT && (beam.dir == Dir::NORTH || beam.dir == Dir::SOUTH)) ||
        (map.at(i).at(j) == Tile::SPL_HOR && (beam.dir == Dir::EAST || beam.dir == Dir::WEST))) {
      continue;
    }
    //  PrintMaps(map, beam_map, beams, latest);
  }
  return CountEnergy(beam_map);
}

}  // namespace

namespace fmt {

template<>
struct formatter<Dir> : formatter<char> {
  auto format(Dir dir, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(dir), ctx);
  }
};
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

namespace {

void PrintMap(const Map &map,
              const BeamMap &beam_map,
              const Beams &pending,
              absl::flat_hash_set<Pos> &latest) {
  u32 height = beam_map.size();
  u32 width = beam_map[0].size();
  fmt::text_style color_default = fmt::fg(fmt::color::gray);
  fmt::text_style color_highlight = fmt::emphasis::bold | fmt::fg(fmt::color::cyan);
  fmt::text_style color_next = fmt::emphasis::bold | fmt::fg(fmt::color::yellow);
  fmt::text_style color_latest = fmt::emphasis::bold | fmt::fg(fmt::color::hot_pink);
  fmt::text_style color;
  absl::flat_hash_set<Pos> highlights{};
  absl::c_transform(pending, std::inserter(highlights, highlights.begin()), [](const auto &b) { return b.pos; });
  for (i32 i = 0; i < height; i++) {
    for (i32 j = 0; j < width; j++) {
      Pos pos{i, j};
      if (latest.contains(pos)) {
        color = color_latest;
      } else if (highlights.contains(pos)) {
        if (pending.at(0).pos == pos || pending.size() > 1 && pending.at(1).pos == pos) {
          color = color_next;
        } else {
          color = color_highlight;
        }
      } else {
        color = color_default;
      }
      const auto &bm = beam_map.at(i).at(j);
      if (map.at(i).at(j) != Tile::EMPTY) {
        fmt::print(color, "{}", map.at(i).at(j));
        continue;
      }
      if (bm.empty()) {
        fmt::print(color, ".");
        continue;
      }
      if (bm.size() == 1) {
        fmt::print(color, "{}", *bm.begin());
        continue;
      }
      fmt::print(color, "{}", bm.size());
    }
    fmt::print("\n");
  }
  fmt::print("\n");
  latest.clear();
}

}  // namespace

template<>
auto advent<2023, 16>::solve() -> Result {
  std::string input = GetInput();
  Map map = aoc::util::TokenizeInput<std::vector<Tile>>(input, [](auto line) {
    std::vector<Tile> row{line.size()};
    for (i32 i = 0; i < line.size(); i++) {
      row[i] = ParseTile(line.at(i));
    }
    return row;
  });

  // Part 1
  Beams beams{{{0, -1}, Dir::EAST}};
  u64 part1{Energize(map, {{0, -1}, Dir::EAST})};

  // Part 2
  u64 part2{part1};
  u32 height = map.size();
  u32 width = map.at(0).size();
  u64 energy = Energize(map, {{0, width}, Dir::WEST});
  if (energy > part2) part2 = energy;
  for (i32 i = 1; i < height; i++) {
    energy = Energize(map, {{i, -1}, Dir::EAST});
    if (energy > part2) part2 = energy;
    energy = Energize(map, {{i, width}, Dir::WEST});
    if (energy > part2) part2 = energy;
  }
  for (i32 j = 0; j < width; j++) {
    energy = Energize(map, {{-1, j}, Dir::SOUTH});
    if (energy > part2) part2 = energy;
    energy = Energize(map, {{height, j}, Dir::NORTH});
    if (energy > part2) part2 = energy;
  }

  return aoc::result(part1, part2);
}