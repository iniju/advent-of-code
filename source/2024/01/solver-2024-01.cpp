#include <aoc.hpp>

#include <fmt/format.h>

namespace {

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 01>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());

  std::vector<i32> list1, list2;
  for (auto line : lines) {
    i32 a, b;
    CHECK(scn::scan(line, "{} {}", a, b)) << "Unable to parse input `" << line << "`.";
    list1.push_back(a);
    list2.push_back(b);
  }

  // Part 1
  u64 dist = 0;
  absl::c_sort(list1);
  absl::c_sort(list2);
  for (i64 i = 0; i <list1.size(); i++) {
    dist += labs(list1.at(i) - list2.at(i));
  }
  u64 part1 = dist;

  // Part 2
  u64 similarity = 0;
  absl::flat_hash_map<i32, u64> appears;
  for (auto n : list2) {
    appears[n]++;
  }
  for (auto n : list1) {
    if (appears.contains(n)) similarity += static_cast<u64>(n) * appears.at(n);
  }
  u64 part2 = similarity;

  return aoc::result(part1, part2);
}