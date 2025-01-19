#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Report = std::vector<i32>;
using Reports = std::vector<Report>;

bool IsSafe(const Report &report) {
  i32 init_delta = report.at(1) - report.at(0);
  if (init_delta == 0 || init_delta > 3 || init_delta < -3) return false;
  for (i32 i = 2; i < report.size(); i++) {
    i32 delta = report.at(i) - report.at(i - 1);
    if (delta == 0 || delta > 3 || delta < -3 || init_delta * delta < 0) return false;
  }
  return true;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 02>::solve() -> Result {
  std::string input = GetInput();
  Reports reports = aoc::util::TokenizeInput<Report>(
      input,
      [](absl::string_view line) {
        Report report;
        aoc::util::ScanList(line, report);
        return report;
      });

  // Part 1
  u64 part1 = 0;
  for (const auto &report : reports) {
    if (IsSafe(report)) {
      part1++;
    }
  }

  // Part 2
  u64 part2 = 0;
  for (const auto &report : reports) {
    if (IsSafe(report)) {
      part2++;
    } else {
      for (i32 f = 0; f < report.size(); f++) {
        Report fixed(report.begin(), report.end());
        fixed.erase(fixed.begin() + f);
        if (IsSafe(fixed)) {
          part2++;
          break;
        }
      }
    }
  }

  return aoc::result(part1, part2);
}