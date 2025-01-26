#include <aoc.hpp>

#include <fmt/format.h>

namespace {

enum class Tile : char {
  EMPTY = '.',
  WALL = '#',
  BOX = 'O',
  WBOX_LEFT = '[',
  WBOX_RIGHT = ']',
  ROBOT = '@'
};
using Row = std::vector<Tile>;
using Map = std::vector<Row>;
using Moves = std::vector<aoc::Dir>;
using PushResult = std::pair<bool, std::vector<aoc::Pos>>;

void MoveRobot(Map &map, aoc::Pos &robot, aoc::Dir dir) {
  aoc::Pos move = aoc::MoveDir(dir);
  aoc::Pos new_pos = robot + move;
  switch (map.at(new_pos.i).at(new_pos.j)) {
    case Tile::ROBOT: CHECK(false) << "Robot can't bump on itself.";
    case Tile::WALL: return;
    case Tile::WBOX_LEFT:
    case Tile::WBOX_RIGHT: CHECK(false) << "Unexpected box.";
    case Tile::BOX: {
      aoc::Pos last_box = new_pos;
      Tile after_boxes;
      do {
        last_box = last_box + move;
        after_boxes = map.at(last_box.i).at(last_box.j);
      } while (after_boxes == Tile::BOX);
      if (after_boxes == Tile::WALL) return;
      map[last_box.i][last_box.j] = Tile::BOX;
    }
    case Tile::EMPTY:map[robot.i][robot.j] = Tile::EMPTY;
      map[new_pos.i][new_pos.j] = Tile::ROBOT;
      robot = new_pos;
  }
}

aoc::Pos FindRobot(const Map &map, u32 height, u32 width) {
  for (u32 i = 0; i < height; i++) {
    for (u32 j = 0; j < width; j++) {
      if (map.at(i).at(j) == Tile::ROBOT) {
        return {i, j};
      }
    }
  }
  CHECK(false) << "No robot found in the map.";
}

u64 CalcBoxGPS(const Map &map, u32 height, u32 width) {
  u64 result = 0;
  for (u32 i = 0; i < height; i++) {
    for (u32 j = 0; j < width; j++) {
      if (map.at(i).at(j) == Tile::BOX || map.at(i).at(j) == Tile::WBOX_LEFT) result += 100 * i + j;
    }
  }
  return result;
}

void WidenMap(const Map &map, Map &wide_map) {
  wide_map.clear();
  for (const auto &row : map) {
    wide_map.emplace_back();
    for (Tile tile : row) {
      switch (tile) {
        case Tile::EMPTY:
        case Tile::WALL: {
          wide_map.back().push_back(tile);
          wide_map.back().push_back(tile);
          break;
        }
        case Tile::BOX: {
          wide_map.back().push_back(Tile::WBOX_LEFT);
          wide_map.back().push_back(Tile::WBOX_RIGHT);
          break;
        }
        case Tile::ROBOT: {
          wide_map.back().push_back(Tile::ROBOT);
          wide_map.back().push_back(Tile::EMPTY);
          break;
        }
        default:CHECK(false) << "Unexpected tile '" << aoc::ToUnderlying(tile) << "'.";
      }
    }
  }
}

PushResult CanPushNS(const Map &map, std::vector<aoc::Pos> &from, aoc::Dir dir) {
  aoc::Pos move = aoc::MoveDir(dir);
  std::vector<aoc::Pos> pushed;
  std::vector<aoc::Pos> push(from.begin(), from.end());
  absl::flat_hash_set<aoc::Pos> new_push;
  while (!push.empty()) {
    new_push.clear();
    for (const auto &pos : push) {
      pushed.emplace_back(pos.i, pos.j);
      auto new_pos = pos + move;
      auto tile = map.at(new_pos.i).at(new_pos.j);
      switch (tile) {
        case Tile::WALL: return {false, {}};
        case Tile::EMPTY: break;
        case Tile::WBOX_LEFT: {
          new_push.insert(new_pos);
          new_push.emplace(new_pos.i, new_pos.j + 1);
          break;
        }
        case Tile::WBOX_RIGHT: {
          new_push.insert(new_pos);
          new_push.emplace(new_pos.i, new_pos.j - 1);
          break;
        }
        default: CHECK(false) << "Unexpected tile '" << aoc::ToUnderlying(tile) << "'.";
      }
    }
    push.clear();
    for (const auto &p : new_push) push.emplace_back(p.i, p.j);
  }
  return {true, pushed};
}

PushResult CanPushEW(const Map &map, const aoc::Pos &from, aoc::Dir dir) {
  aoc::Pos move = aoc::MoveDir(dir);
  std::vector<aoc::Pos> pushed;
  aoc::Pos last_box = from;
  Tile after_boxes;
  do {
    pushed.push_back(last_box);
    last_box = last_box + move;
    after_boxes = map.at(last_box.i).at(last_box.j);
  } while (after_boxes == Tile::WBOX_LEFT || after_boxes == Tile::WBOX_RIGHT);
  switch (after_boxes) {
    case Tile::WALL: return {false, {}};
    case Tile::EMPTY: break;
    default: CHECK(false) << "Unexpected tile '" << aoc::ToUnderlying(after_boxes) << "'.";
  }
  return {true, pushed};
}

void WMoveRobot(Map &map, aoc::Pos &robot, aoc::Dir dir) {
  aoc::Pos move = aoc::MoveDir(dir);
  aoc::Pos new_pos = robot + move;
  auto tile = map.at(new_pos.i).at(new_pos.j);
  switch (tile) {
    case Tile::ROBOT: CHECK(false) << "Robot can't bump on itself.";
    case Tile::BOX: CHECK(false) << "Unexpected box.";
    case Tile::WALL: return;
    case Tile::WBOX_LEFT:
    case Tile::WBOX_RIGHT: {
      PushResult push_result;
      std::vector<aoc::Pos> from;
      if (dir == aoc::Dir::N || dir == aoc::Dir::S) {
        if (tile == Tile::WBOX_LEFT) {
          from.push_back(new_pos);
          from.emplace_back(new_pos.i, new_pos.j + 1);
        } else {
          from.emplace_back(new_pos.i, new_pos.j - 1);
          from.push_back(new_pos);
        }
        push_result = CanPushNS(map, from, dir);
      } else {
        push_result = CanPushEW(map, new_pos, dir);
      }
      if (!push_result.first) return;
      for (i32 p = static_cast<i32>(push_result.second.size()) - 1; p >= 0; p--) {
        auto box_from = push_result.second.at(p);
        auto box_to = box_from + move;
        map[box_to.i][box_to.j] = map[box_from.i][box_from.j];
        map[box_from.i][box_from.j] = Tile::EMPTY;
      }
    }
    case Tile::EMPTY:map[robot.i][robot.j] = Tile::EMPTY;
      map[new_pos.i][new_pos.j] = Tile::ROBOT;
      robot = new_pos;
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
auto advent<2024, 15>::solve() -> Result {
  std::vector<absl::string_view> parts = absl::StrSplit(input, "\n\n", absl::SkipWhitespace());
  Map map = aoc::util::TokenizeInput<Row>(parts.at(0), [](auto line) {
    Row row;
    absl::c_transform(line, std::back_inserter(row), [](char ch) {
      switch (ch) {
        case '.': return Tile::EMPTY;
        case '#': return Tile::WALL;
        case 'O': return Tile::BOX;
        case '@': return Tile::ROBOT;
        default: CHECK(false) << "Unknown map tile '" << ch << "'.";
      }
    });
    return row;
  });
  Moves moves;
  for (char ch : parts.at(1)) {
    if (ch == '\n' || ch == '\r') continue;
    moves.push_back(aoc::ParseDir(ch));
  }
  u32 height = map.size(), width = map.at(0).size();
  aoc::Pos robot = FindRobot(map, height, width);

  // Part 1
  Map map1{map};
  for (auto dir : moves) {
    MoveRobot(map1, robot, dir);
  }
  u64 part1 = CalcBoxGPS(map1, height, width);

  // Part 2
  Map map2;
  WidenMap(map, map2);
  width = map2.at(0).size();
  robot = FindRobot(map2, height, width);
  for (auto dir : moves) {
    WMoveRobot(map2, robot, dir);
  }
  u64 part2 = CalcBoxGPS(map2, height, width);

  return aoc::result(part1, part2);
}