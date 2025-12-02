#include <aoc.h>

#include <fmt/format.h>

namespace {

using Id = u64;
using Range = std::pair<Id, Id>;
using Ranges = std::vector<Range>;
using IdSet = absl::flat_hash_set<Id>;

u64 MakeWhole(u64 pattern, u32 splits) {
  u64 result = pattern;
  auto multiplier = aoc::kTenPowers[aoc::util::NumDigits(pattern)];
  for (auto i = 1; i < splits; i++) result = result * multiplier + pattern;
  return result;
}
Id GetPattern(u64 id, u32 num_digits, u32 splits) {
  return id / aoc::kTenPowers[num_digits - num_digits / splits];
}

void ScanRangeWithSplits(u64 start, u64 end, u32 num_digits, u32 splits, IdSet& result) {
  if (num_digits % splits) return;
  u64 pattern_start = GetPattern(start, num_digits, splits);
  while (MakeWhole(pattern_start, splits) < start) pattern_start++;

  u64 pattern_end = GetPattern(end, num_digits, splits);
  while (MakeWhole(pattern_end, splits) > end) pattern_end--;

  for (auto pattern = pattern_start; pattern <= pattern_end; pattern++) {
    result.insert(MakeWhole(pattern, splits));
  }
}

void ScanRange(u64 start, u64 end, u32 num_digits, u32 max_splits, IdSet& result) {
  for (u32 splits = 2; splits <= max_splits; splits++) {
    ScanRangeWithSplits(start, end, num_digits, splits, result);
  }
}

u64 CountInvalid(u64 start, u64 end, bool all_splits) {
  IdSet invalid_ids;
  u32 start_num_digits = aoc::util::NumDigits(start);
  u32 end_num_digits = aoc::util::NumDigits(end);

  while (start_num_digits < end_num_digits) {
    ScanRange(
        start, aoc::kTenPowers[start_num_digits] - 1, start_num_digits,
        all_splits ? start_num_digits : 2, invalid_ids);
    start = aoc::kTenPowers[start_num_digits];
    start_num_digits++;
  }
  ScanRange(start, end, start_num_digits, all_splits ? start_num_digits : 2, invalid_ids);

  return absl::c_accumulate(invalid_ids, 0ll);
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 02>::solve() -> Result {
  Ranges ranges;
  for (auto line : input | std::views::split(',')) {
    std::vector<Id> result;
    aoc::util::FastScanList(line, result, "-");
    ranges.emplace_back(result[0], result[1]);
  }

  // Part 1 & Part 2
  u64 part1 = 0, part2 = 0;
  for (const Range& range : ranges) {
    part1 += CountInvalid(range.first, range.second, false);
    part2 += CountInvalid(range.first, range.second, true);
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 02>::PartOne() -> std::string { return "21139440284"; }

template<> auto advent<2025, 02>::PartTwo() -> std::string { return "38731915928"; }
