#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using AntennaList = std::vector<aoc::Pos>;
using AntennaSet = absl::flat_hash_set<aoc::Pos>;
using Antennas = absl::flat_hash_map<char, AntennaList>;

bool TryAddAntinode(u64 height, u64 width, const aoc::Pos& antinode, AntennaSet& antinode_set) {
  if (aoc::util::IsOutOfMap(height, width, antinode)) {
    return false;
  }
  antinode_set.insert(antinode);
  return true;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 8>::solve() -> Result {
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Antennas antennas;
  for (u32 i = 0; i < height; i++) {
    for (u32 j = 0; j < width; j++) {
      char ch = lines.at(i).at(j);
      if (ch == '.') continue;
      antennas[ch].emplace_back(i, j);
    }
  }

  // Part 1 & Part 2
  absl::flat_hash_set<aoc::Pos> antinodes1, antinodes2;
  for (const auto& [ch, locs] : antennas) {
    antinodes2.insert(locs.at(locs.size() - 1));
    for (u32 i = 0; i < locs.size() - 1; i++) {
      const auto& a = locs.at(i);
      antinodes2.insert(a);
      for (u32 j = i + 1; j < locs.size(); j++) {
        const auto& b = locs.at(j);
        auto delta = b - a;
        auto c = locs.at(i) - delta;
        if (TryAddAntinode(height, width, c, antinodes1)) {
          antinodes2.insert(c);
          do {
            c = c - delta;
          } while (TryAddAntinode(height, width, c, antinodes2));
        }
        auto d = locs.at(j) + delta;
        if (TryAddAntinode(height, width, d, antinodes1)) {
          antinodes2.insert(d);
          do {
            d = d + delta;
          } while (TryAddAntinode(height, width, d, antinodes2));
        }
      }
    }
  }
  u64 part1 = antinodes1.size();
  u64 part2 = antinodes2.size();

  return aoc::result(part1, part2);
}