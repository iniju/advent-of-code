#include <aoc.h>

#include <fmt/format.h>

namespace {

using Column = u8;
using Schematic = std::vector<Column>;
using Schematics = std::vector<Schematic>;

std::pair<bool, Schematic> ParseKey(absl::string_view block) {
  std::vector<absl::string_view> rows = absl::StrSplit(block, "\n", absl::SkipWhitespace());
  Schematic result{};
  bool is_key = block.at(0) == '.';
  char opposite = is_key ? '#' : '.';
  for (u32 j = 0; j < 5; j++) {
    for (u32 i = 0; i < 7; i++) {
      if (rows.at(i).at(j) == opposite) {
        result.push_back(is_key ? 5 - i + 1 : i - 1);
        break;
      }
    }
  }
  return std::make_pair(is_key, result);
}

bool IsFit(const auto &lock, const auto &key) {
  for (u32 i = 0; i < 5; i++) if (lock.at(i) + key.at(i) > 5) return false;
  return true;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 25>::solve() -> Result {
  std::vector<absl::string_view> blocks = absl::StrSplit(input, "\n\n", absl::SkipWhitespace());

  Schematics keys, locks;
  for (auto block : blocks) {
    auto [is_key, schematic] = ParseKey(block);
    if (is_key) keys.push_back(schematic); else locks.push_back(schematic);
  }

  // Part 1
  u64 part1 = 0;
  for (const auto &key : keys) {
    for (const auto &lock : locks) {
      if (IsFit(lock, key)) part1++;
    }
  }

  // Part 2
  u64 part2 = 0;

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 25>::PartOne() -> std::string { return "3619"; }

template<> auto advent<2024, 25>::PartTwo() -> std::string { return "0"; }
