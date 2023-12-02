#include <aoc.hpp>
#include <absl/container/flat_hash_map.h>
#include <ostream>

namespace {
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
      std::vector<absl::string_view> counts_list = absl::StrSplit(set_str, ", ");
      for (auto count_str : counts_list) {
        CubeCount cube_count{};
        std::string color;
        auto red_count = scn::scan(count_str, "{}{}", cube_count.count, color);
        cube_count.color = parseColor(color);
        assert(color != CubeColor.ERROR);
        cube_set.push_back(cube_count);
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