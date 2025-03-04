#include <aoc.hpp>

namespace {

using AntennaList = std::vector<i32>;
using AntennaSet = absl::flat_hash_set<i32>;
using Antennas = absl::flat_hash_map<char, AntennaList>;

bool TryAddAntinode(
    const u32 height, const u32 width, const aoc::Pos& pos, const i32 antinode,
    AntennaSet& antinode_set) {
  if (aoc::util::IsOutOfMap(height, width, pos)) {
    return false;
  }
  antinode_set.insert(antinode);
  return true;
}

}  // namespace

namespace fmt {}  // namespace fmt

template<> auto advent<2024, 8>::solve() -> Result {
  const u32 width = input.find_first_of('\n');
  const u32 height = (input.size() + 1) / width - 1;
  Antennas antennas;
  auto it = input.begin();
  while (it < input.end()) {
    it = std::find_if(it, input.end(), [](const char c) { return c != '.' && c != '\n'; });
    if (it == input.end()) break;
    antennas[*it].push_back(std::distance(input.begin(), it));
    ++it;
  }

  // Part 1 & Part 2
  AntennaSet antinodes1, antinodes2;
  for (const auto& [_, locs] : antennas) {
    antinodes2.insert(locs[locs.size() - 1]);
    for (u32 i = 0; i < locs.size() - 1; i++) {
      const auto a = locs.at(i);
      antinodes2.insert(a);
      auto pos_a = aoc::Pos::FromLinearMap(a, width);
      for (u32 j = i + 1; j < locs.size(); j++) {
        const auto b = locs.at(j);
        auto pos_b = aoc::Pos::FromLinearMap(b, width);
        auto pos_delta = pos_b - pos_a;
        const auto delta = b - a;
        auto c = a - delta;
        if (auto pos_c = pos_a - pos_delta; TryAddAntinode(height, width, pos_c, c, antinodes1)) {
          antinodes2.insert(c);
          do {
            c = c - delta;
            pos_c = pos_c - pos_delta;
          } while (TryAddAntinode(height, width, pos_c, c, antinodes2));
        }
        auto d = b + delta;
        if (auto pos_d = pos_b + pos_delta; TryAddAntinode(height, width, pos_d, d, antinodes1)) {
          antinodes2.insert(d);
          do {
            d = d + delta;
            pos_d = pos_d + pos_delta;
          } while (TryAddAntinode(height, width, pos_d, d, antinodes2));
        }
      }
    }
  }
  const u64 part1 = antinodes1.size();
  const u64 part2 = antinodes2.size();

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 8>::PartOne() -> std::string { return "273"; }

template<> auto advent<2024, 8>::PartTwo() -> std::string { return "1017"; }
