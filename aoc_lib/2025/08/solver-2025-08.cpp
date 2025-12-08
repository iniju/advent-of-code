#include <aoc.h>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <fmt/format.h>
#include <Eigen/Core>

namespace {

using Junction = Eigen::Array<u64, 3, 1>;
using Junctions = std::vector<Junction>;
using Distances = absl::btree_map<u64, std::pair<u32, u32>>;

}  // namespace

namespace fmt {

template<typename T>
struct formatter<T, std::enable_if_t<std::is_base_of_v<Eigen::Array<T, 3, 1>, T>, char>>
    : ostream_formatter {};

}  // namespace fmt

template<>
auto advent<2025, 8>::solve() -> Result {
  Junctions junctions;
  for (auto line : input | std::views::split('\n')) {
    std::vector<u64> numbers;
    aoc::util::FasterScanList(line, numbers, 1);
    junctions.push_back(Eigen::Map<Junction>(numbers.data()));
    const auto& foo = junctions.back();
  }

  Distances distances;
  for (u32 i = 0; i < junctions.size() - 1; i++) {
    for (u32 j = i + 1; j < junctions.size(); j++) {
      auto tmp = (junctions[i] - junctions[j]);
      u64 d = (tmp * tmp).sum();
      CHECK(!distances.contains(d)) << "Distance " << d << "found twice.";
      distances[d] = std::make_pair(i, j);
    }
  }

  // Part 1 & 2
  u64 part1 = 1;
  u64 part2;
  std::vector<u32> junction_to_set;
  absl::flat_hash_map<u32, absl::btree_set<u32>> sets;
  for (u32 i = 0; i < junctions.size(); i++) {
    junction_to_set.push_back(i);
    sets[i].insert(i);
  }

  u32 connections = 0;
  for (auto [d, pair] : distances) {
    if (++connections == 1001) {
      std::priority_queue<u32> set_sizes;
      for (auto [_, set] : sets) {
        set_sizes.push(set.size());
      }
      for (u32 i = 0; i < 3; i++) {
        part1 *= set_sizes.top();
        set_sizes.pop();
      }
    }
    u32 set1_id = junction_to_set[pair.first];
    u32 set2_id = junction_to_set[pair.second];
    if (set1_id == set2_id) continue;
    for (auto j : sets[set2_id]) {
      sets[set1_id].insert(j);
      junction_to_set[j] = set1_id;
    }
    sets.erase(set2_id);
    if (sets.size() == 1) {
      part2 = junctions[pair.first][0] * junctions[pair.second][0];
      break;
    }
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 8>::PartOne() -> std::string { return "175500"; }

template<> auto advent<2025, 8>::PartTwo() -> std::string { return "6934702555"; }
