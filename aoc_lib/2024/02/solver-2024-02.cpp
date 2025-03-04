#include <aoc.h>

namespace {

using Report = std::vector<i32>;
using Reports = std::vector<Report>;

bool IsSafe(const Report& report, const u32 skip = std::numeric_limits<u32>::max()) {
  const i32 init_delta = report[skip <= 1 ? 2 : 1] - report[skip == 0 ? 1 : 0];
  if (init_delta == 0 || init_delta > 3 || init_delta < -3) return false;
  u32 prev = skip <= 1 ? 2 : 1;
  i32 i = skip <= 2 ? 3 : 2;
  while (i < report.size()) {
    if (const i32 delta = report[i] - report[prev];
        delta == 0 || delta > 3 || delta < -3 || init_delta * delta < 0)
      return false;
    prev = i;
    i += skip == i + 1 ? 2 : 1;
  }
  return true;
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2024, 02>::solve() -> Result {
  i32 x;
  Reports reports;
  for (const auto line : input | std::views::split('\n')) {
    aoc::util::FasterScanList(line, reports.emplace_back());
  }

  // Part 1
  u64 part1 = 0;
  std::vector<bool> safe(reports.size(), false);
  for (u32 i = 0; i < reports.size(); i++) {
    if (IsSafe(reports[i])) {
      part1++;
      safe[i] = true;
    }
  }

  // Part 2
  u64 part2 = part1;
  for (u32 i = 0; i < reports.size(); i++) {
    if (safe.at(i)) {
      continue;
    }
    const auto& report = reports[i];
    for (i32 f = 0; f < report.size(); f++) {
      if (IsSafe(report, f)) {
        part2++;
        break;
      }
    }
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 2>::PartOne() -> std::string { return "279"; }

template<> auto advent<2024, 2>::PartTwo() -> std::string { return "343"; }
