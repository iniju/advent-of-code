#include <aoc.hpp>

#include <fmt/format.h>
#include <cfenv>
#include <re2/re2.h>
#pragma STDC FENV_ACCESS ON

namespace {

RE2 SpacesPattern = ("\\s+");

std::vector<u16> ParsePart1Line(absl::string_view line) {
  std::vector<absl::string_view> splits = absl::StrSplit(line, ':', absl::SkipWhitespace());
  CHECK(splits.size() == 2);
  std::vector<u16> numbers;
  CHECK(scn::scan_list(splits.at(1), numbers)) << "Could not parse numbers '" << line << "'.";
  return numbers;
}
u64 ParsePart2Line(absl::string_view line) {
  std::string kerning_fixed{line};
  RE2::GlobalReplace(&kerning_fixed, SpacesPattern, {});
  std::vector<absl::string_view> splits = absl::StrSplit(kerning_fixed, ':', absl::SkipWhitespace());
  CHECK(splits.size() == 2);
  u64 number{};
  CHECK(scn::scan_default(splits.at(1), number)) << "Could not parse number '" << splits.at(1) << "'.";
  return number;
}

std::pair<u64, u64> CalculateWinningLimits(u64 time, u64 distance) {
  auto t = (double) time;
  auto d = (double) distance;
  double sqr_comp = sqrt(t * t - 4 * d);
  double t1 = (t - sqr_comp) / 2.0;
  if (t1 < 0) t1 = 0.0;
  double t2 = (t + sqr_comp) / 2.0;
  auto old_round_direction = fegetround();
  fesetround(FE_UPWARD);
  double tr1 = ceil(t1);
  if (tr1 == t1) tr1 += 1.0;
  u64 ti1 = lrint(tr1);
  fesetround(FE_DOWNWARD);
  double tr2 = floor(t2);
  if (tr2 == t2) tr2 -= 1.0;
  u64 ti2 = lrint(tr2);
  fesetround(old_round_direction);
  return {ti1, ti2};
}

}  // namespace

template<>
auto advent<2023, 6>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());

  // Part 1
  std::vector<u16> times = ParsePart1Line(lines.at(0));
  std::vector<u16> distances = ParsePart1Line(lines.at(1));
  CHECK(times.size() == distances.size());
  u64 part1 = 1;
  for (int i = 0; i < times.size(); i++) {
    auto winning_limits = CalculateWinningLimits(times.at(i), distances.at(i));
    part1 *= winning_limits.second - winning_limits.first + 1;
  }

  // Part 2
  u64 actual_time = ParsePart2Line(lines.at(0));
  u64 actual_distance = ParsePart2Line(lines.at(1));
  auto winning_limit = CalculateWinningLimits(actual_time, actual_distance);
  u64 part2 = winning_limit.second - winning_limit.first + 1;

  return aoc::result(part1, part2);
}