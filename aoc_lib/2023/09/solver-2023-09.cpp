#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>
#include <re2/re2.h>

namespace {

using History = std::deque<i64>;

History GetNextSequence(const History &sequence) {
  History result{};
  for (int i = 0; i < sequence.size() - 1; i++) {
    result.push_back(sequence[i + 1] - sequence[i]);
  }
  return result;
}

}  // namespace

template<>
auto advent<2023, 9>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  std::vector<History> histories{};
  absl::c_for_each(lines, [&histories](absl::string_view line) {
    histories.emplace_back();
    CHECK(scn::scan_list(line, histories.back()));
  });

  u64 part1 = 0;
  u64 part2 = 0;
  for (const auto &history : histories) {
    std::vector<History> sequences;
    sequences.push_back(history);
    while (absl::c_any_of(sequences.back(), [](i64 x) { return x != 0; })) {
      sequences.emplace_back(GetNextSequence(sequences.back()));
    }

    // Part 1 & 2
    sequences.back().push_back(0);
    for (i64 i = static_cast<i64>(sequences.size() - 2); i >= 0; i--) {
      sequences[i].emplace_back(sequences[i].back() + sequences[i + 1].back());
      sequences[i].emplace_front(sequences[i].front() - sequences[i + 1].front());
    }
    part1 += sequences[0].back();
    part2 += sequences[0].front();
  }

  return aoc::result(part1, part2);
}