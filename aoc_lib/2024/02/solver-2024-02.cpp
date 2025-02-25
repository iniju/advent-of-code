#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Report = std::vector<i32>;
using Reports = std::vector<Report>;

inline bool IsSafe(const Report &report, u32 skip = std::numeric_limits<u32>::max()) {
  i32 init_delta = report.at(skip <= 1 ? 2 : 1) - report.at(skip == 0 ? 1 : 0);
  if (init_delta == 0 || init_delta > 3 || init_delta < -3) return false;
  u32 prev = skip <= 1 ? 2 : 1;
  i32 i = skip <= 2 ? 3 : 2;
  while (i < report.size()) {
    i32 delta = report.at(i) - report.at(prev);
    if (delta == 0 || delta > 3 || delta < -3 || init_delta * delta < 0) return false;
    prev = i;
    i += skip == i + 1 ? 2 : 1;
  }
  return true;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 02>::solve() -> Result {
  i32 x;
  Reports reports;
  for (auto line : std::ranges::split_view(input, '\n')) {
    reports.emplace_back();
    const char *ptr = line.data();
    auto line_end = ptr + line.size();
    while (ptr < line_end) {
      auto result = fast_float::from_chars(ptr, line_end, x);
      CHECK(result.ec == std::errc()) << "Couldn't parse '" << ptr << "'.";
      reports.back().push_back(x);
      ptr = result.ptr + 1;
    }
  }
//  std::ranges::copy(
//      input | std::views::split('\n') | std::views::transform([](auto line) -> Report {
//        Report report;
//        i32 x;
//        const char *ptr = line.data();
//        auto line_end = ptr + line.size();
//        while (ptr < line_end) {
//          auto result = fast_float::from_chars(ptr, line_end, x);
//          CHECK(result.ec == std::errc()) << "Couldn't parse '" << ptr << "'.";
//          report.push_back(x);
//          ptr = result.ptr + 1;
//        }
//        return report;
//      }),
//      reports.begin()
//  );

  // Part 1
  u64 part1 = 0;
  std::vector<bool> safe(reports.size(), false);
  for (u32 i = 0; i < reports.size(); i++) {
    if (IsSafe(reports.at(i))) {
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
    const auto &report = reports.at(i);
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
