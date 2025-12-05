#include <aoc.h>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

using Id = u64;
using Ids = std::vector<Id>;
using IdRanges = absl::btree_map<Id, Id>;

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 05>::solve() -> Result {
  Ids ids;
  IdRanges ranges;
  bool ranges_done = false;
  for (auto line : input | std::views::split('\n')) {
    if (line.empty()) {
      ranges_done = true;
      continue;
    }
    const auto line_start = std::ranges::data(line);
    const auto line_end = line_start + std::ranges::size(line);
    if (!ranges_done) {
      u64 from, to;
      auto [ptr, _] = fast_float::from_chars(line_start, line_end, from);
      fast_float::from_chars(ptr + 1, line_end, to);
      if (!ranges.contains(from) || to > ranges[from]) ranges[from] = to;
      continue;
    }
    Id id;
    fast_float::from_chars(line_start, line_end, id);
    ids.emplace_back(id);
  }

  // Part 1
  u64 part1 = 0;
  for (auto id : ids) {
    for (auto it = ranges.begin(); it != ranges.upper_bound(id); it++) {
      if (id <= it->second) {
        part1++;
        break;
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  IdRanges disjoint;
  for (auto [from, to] : ranges) {
    if (disjoint.empty()) {
      disjoint[from] = to;
      continue;
    }
    bool skip = false;
    u64 new_from = from;
    for (auto& it : disjoint) {
      if (it.second < new_from) continue;
      if (it.second >= to) {
        skip = true;
        break;
      }
      new_from = it.second + 1;
    }
    if (skip) continue;
    disjoint[new_from] = to;
  }
  for (auto [from, to] : disjoint) part2 += to - from + 1;

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 05>::PartOne() -> std::string { return "617"; }

template<> auto advent<2025, 05>::PartTwo() -> std::string { return "338258295736104"; }
