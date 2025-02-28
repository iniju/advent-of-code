#include <aoc.hpp>

namespace {

using AntennaList = std::vector<i32>;
using AntennaSet = absl::flat_hash_set<i32>;
using Antennas = absl::flat_hash_map<char, AntennaList>;

bool TryAddAntinode(i32 height, i32 width, i32 i, i32 j, i32 antinode, AntennaSet& antinode_set) {
  if (i < 0 || i >= height || j  < 0 || j >= width) {
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
  const i32 width = static_cast<i32>(input.find_first_of('\n'));
  const i32 height = static_cast<i32>((input.size() + 1)/ width - 1);
  Antennas antennas;
  auto it = input.begin();
  while (it < input.end()) {
    it = std::find_if(it, input.end(), [](char c) { return c != '.' && c != '\n'; });
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
      auto [ai, aj] = std::div(a, width + 1);
      for (u32 j = i + 1; j < locs.size(); j++) {
        const auto b = locs.at(j);
        auto [bi, bj] = std::div(b, width + 1);
        auto delta_i = bi - ai;
        auto delta_j = bj - aj;
        auto delta = b - a;
        auto c = a - delta;
        auto ci = ai - delta_i;
        auto cj = aj - delta_j;
        if (TryAddAntinode(height, width, ci, cj, c, antinodes1)) {
          antinodes2.insert(c);
          do {
            c = c - delta;
            ci = ci - delta_i;
            cj = cj - delta_j;
          } while (TryAddAntinode(height, width, ci, cj, c, antinodes2));
        }
        auto d = b + delta;
        auto di = bi + delta_i;
        auto dj = bj + delta_j;
        if (TryAddAntinode(height, width, di, dj, d, antinodes1)) {
          antinodes2.insert(d);
          do {
            d = d + delta;
            di = di + delta_i;
            dj = dj + delta_j;
          } while (TryAddAntinode(height, width, di, dj, d, antinodes2));
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
