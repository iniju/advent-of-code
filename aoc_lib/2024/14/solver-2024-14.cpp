#include <aoc.h>

#include <fmt/format.h>
#include <re2/re2.h>

namespace {

RE2 InputPattern(R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+))");
using Robot = std::pair<aoc::Pos, aoc::Pos>;
using Robots = std::vector<Robot>;

void MoveRobot(u32 height, u32 width, u32 seconds, Robot &robot) {
  aoc::Pos move = robot.second;
  move = move * seconds;
  robot.first.i = (robot.first.i + move.i) % height;
  if (robot.first.i < 0) robot.first.i += height;
  robot.first.j = (robot.first.j + move.j + width) % width;
  if (robot.first.j < 0) robot.first.j += width;
}

u64 CalcQuadrants(u32 height, u32 width, const Robots &robots) {
  std::vector<u32> quadrants(4, 0);
  u32 mid_x = width / 2;
  u32 mid_y = height / 2;
  for (const auto &robot : robots) {
    if (robot.first.i < mid_y && robot.first.j < mid_x) quadrants[0]++;
    else if (robot.first.i < mid_y && robot.first.j > mid_x) quadrants[1]++;
    else if (robot.first.i > mid_y && robot.first.j < mid_x) quadrants[2]++;
    else if (robot.first.i > mid_y && robot.first.j > mid_x) quadrants[3]++;
  }
  return quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
}


bool DetectRobotConcentration(u32 fence, u32 required, const Robots &robots) {
  u32 total = robots.size();
  aoc::Pos mid(0, 0);
  for (const auto& robot : robots) {
    mid = mid + robot.first;
  }
  mid.i /= 500;
  mid.j /= 500;
  u32 found = 0;
  fence /= 2;
  for (u32 r = 0; r < robots.size(); r++) {
    if (abs(robots.at(r).first.i - mid.i) > fence || abs(robots.at(r).first.j - mid.j) > fence) continue;
    found++;
    if (found > required) return true;
    if (total - r + found < required) return false;
  }
  return false;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 14>::solve() -> Result {
  Robots robots = aoc::util::TokenizeInput<Robot>(input, [](auto line) {
    i64 px, py, vx, vy;
    CHECK(RE2::FullMatch(line, InputPattern, &px, &py, &vx, &vy)) << "Could not parse '" << line << "'.";
    return std::make_pair(aoc::Pos{py, px}, aoc::Pos{vy, vx});
  });

  u32 height = 103, width = 101;

  // Part 1
  for (auto &robot : robots) {
    MoveRobot(height, width, 100, robot);
  }
  u64 part1 = CalcQuadrants(height, width, robots);

  // Part 2
  u64 s = 100;
  while (true) {
    s++;
    for (auto &robot : robots) {
      MoveRobot(height, width, 1, robot);
    }
    // Exit if at least 300 robots are concentrated in a 40x40 area.
    if (DetectRobotConcentration(40, 300, robots)) break;
  }
  u64 part2 = s;

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 14>::PartOne() -> std::string { return "222062148"; }

template<> auto advent<2024, 14>::PartTwo() -> std::string { return "7520"; }
