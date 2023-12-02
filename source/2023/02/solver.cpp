#include <aoc.hpp>

#include <ostream>
#include <absl/container/flat_hash_map.h>
#include <re2/re2.h>

namespace {
RE2 CubeCountPattern = ("(\\d+) (red|green|blue)(?:, )?");

enum CubeColor {
  ERROR,
  RED,
  GREEN,
  BLUE
};

CubeColor parseColor(absl::string_view color) {
  if (color == "red") return RED;
  if (color == "green") return GREEN;
  if (color == "blue") return BLUE;
  return ERROR;
}

struct CubeCount {
  int count;
  CubeColor color;
};

using CubeSet = std::vector<CubeCount>;

struct Game {
  u64 id{};
  std::vector<CubeSet> sets;
};

}  // namespace detail

template<>
auto advent2023::day02() -> result {
  std::string input = aoc::util::GetInput(year, 2);
  std::vector<absl::string_view> lines = aoc::util::TokenizeInput<absl::string_view>(
      input,
      [](absl::string_view line) { return line; });

  std::vector<Game> games;
  absl::c_transform(lines, std::back_inserter(games), [](auto line) {
    Game game;
    auto ret = scn::scan(line, "Game {}: ", game.id);
    assert(ret);
    std::string sets_part = ret.range_as_string();
    std::vector<absl::string_view> sets_list = absl::StrSplit(sets_part, "; ");
    for (auto set_str : sets_list) {
      CubeSet cube_set;
      int count;
      std::string color;
      absl::string_view ss(set_str);
      while (RE2::Consume(&ss, CubeCountPattern, &count, &color)) {
        cube_set.push_back({count, parseColor(color)});
      }
      game.sets.push_back(cube_set);
    }
    return game;
  });

  // Part 1
  u64 part1 = 0;
  absl::flat_hash_map<CubeColor, int> max_counts{
      {RED, 12}, {GREEN, 13}, {BLUE, 14}
  };
  for (const auto &game : games) {
//    fmt::print("Game {}: {}", game.id, absl::StrJoin(game.sets, "; ", [](std::string* out1, const auto& s) {
//      out1->append(absl::StrJoin(s, ", ", [](std::string* out2, const auto& c) {
//        out2->append(fmt::format("{} {}", c.count, (int)c.color));
//      }));
//    }));
    bool invalid = false;
    for (const auto &cube_set : game.sets) {
      for (const auto &cube_count : cube_set) {
        if (cube_count.count > max_counts.at(cube_count.color)) {
          invalid = true;
          break;
        }
      }
      if (invalid) break;
    }
    if (!invalid) {
      part1 += game.id;
    }
  }

  // Part 2
  u64 part2 = 0;
  for (const auto &game : games) {
    u64 reds = 0;
    u64 blues = 0;
    u64 greens = 0;
    for (const auto &cube_set : game.sets) {
      for (const auto &cube_count : cube_set) {
        switch (cube_count.color) {
          case RED:if (reds < cube_count.count) reds = cube_count.count;
            break;
          case GREEN:if (greens < cube_count.count) greens = cube_count.count;
            break;
          case BLUE:if (blues < cube_count.count) blues = cube_count.count;
            break;
          default:break;
        }
      }
    }
    part2 += reds * greens * blues;
  }

  return aoc::result(part1, part2);
}