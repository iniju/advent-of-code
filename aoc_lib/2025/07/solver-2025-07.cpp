#include <aoc.h>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

using TrapRow = absl::flat_hash_set<u32>;
using Traps = absl::flat_hash_map<u32, TrapRow>;
using Beams = absl::btree_map<u32, u64>;

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 07>::solve() -> Result {
  u32 row = 0;
  u32 start = input.find('S');
  u32 j_start = start;
  u32 j_end = start + 1;
  Traps traps;
  for (auto line : input | std::views::split('\n')) {
    if (row < 2 || row & 1) {
      row++;
      continue;
    }
    for (u32 j = j_start; j < j_end; j++) {
      if (line[j] == '^') {
        traps[row].insert(j);
      }
    }
    j_start--;
    j_end++;
    row++;
  }
  u32 height = row;

  // Part 1
  u64 part1 = 0;
  Beams beams;
  beams[start] = 1;

  for (u32 i = 2; i < height; i += 2) {
    Beams new_beams;
    for (auto [col, num_beams] : beams) {
      if (traps[i].contains(col)) {
        new_beams[col - 1] += num_beams;
        new_beams[col + 1] += num_beams;
        part1++;
      } else {
        new_beams[col] += beams[col];
      }
    }
    std::swap(beams, new_beams);
  }

  // Part 2
  u64 part2 =
      absl::c_accumulate(beams, 0ll, [](auto sum, const auto& pair) { return sum + pair.second; });

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 07>::PartOne() -> std::string { return "1628"; }

template<> auto advent<2025, 07>::PartTwo() -> std::string { return "27055852018812"; }
