#include <aoc.h>

namespace {}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2024, 1>::solve() -> Result {
  std::vector<i32> list1, list2;
  absl::flat_hash_map<i32, u64> appears;
  i32 i;
  for (auto line : input | std::views::split('\n')) {
    const auto line_start = std::ranges::data(line);
    const auto line_end = line_start + std::ranges::size(line);
    auto [ptr, _] = fast_float::from_chars(line_start, line_end, i);
    list1.push_back(i);
    // Integers always separated by 3 spaces.
    fast_float::from_chars(ptr + 3, line_end, i);
    list2.push_back(i);
    appears[i]++;
  }

  // Part 1
  absl::c_sort(list1);
  absl::c_sort(list2);
  auto dist = [](auto a, auto b) { return labs(a - b); };
  auto distances = std::views::zip_transform(dist, list1, list2);
  u64 part1 = std::reduce(distances.cbegin(), distances.cend());

  u64 part2 = 0;
  for (auto n : list1) {
    if (appears.contains(n)) part2 += static_cast<u64>(n) * appears[n];
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 1>::PartOne() -> std::string { return "1189304"; }

template<> auto advent<2024, 1>::PartTwo() -> std::string { return "24349736"; }
