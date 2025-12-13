#include <aoc.h>

#include <fmt/format.h>
#include <Eigen/Core>

namespace {

using Present = Eigen::Matrix3i;
using AllPresents = std::vector<Present>;
using Vector6 = Eigen::Array<u32, 6, 1>;
using Area = std::tuple<u32, u32, Vector6>;
using Areas = std::vector<Area>;

void ParsePresent(std::string_view input, u64& index, AllPresents& all_presents) {
  CHECK(all_presents.size() == input[index] - '0') << "Expected number: " << input[index];
  index += 3;
  Present present;
  for (u32 i = 0; i < 3; i++)
    for (u32 j = 0; j < 3; j++) present(i, j) = input[index + j + i * 4] == '#' ? 1 : 0;
  all_presents.push_back(present);
  index += 13;
}
void ParseArea(std::string_view input, u64& index, Areas& areas) {
  u32 h, w;
  auto [ptr1, _] = std::from_chars(input.data() + index, input.data() + index + 10, w);
  auto [ptr2, _] = std::from_chars(ptr1 + 1, input.data() + index + 10, h);
  Vector6 needed;
  auto ptr = ptr2 + 2;
  for (u32 i = 0; i < 6; i++) {
    u32 x;
    auto result = std::from_chars(ptr, ptr + 10, x);
    needed(i) = x;
    ptr = result.ptr + 1;
  }
  areas.emplace_back(w, h, needed);
  index = std::distance(input.data(), ptr);
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 12>::solve() -> Result {
  u64 index = 0;
  AllPresents all_presents;
  ParsePresent(input, index, all_presents);
  ParsePresent(input, index, all_presents);
  ParsePresent(input, index, all_presents);
  ParsePresent(input, index, all_presents);
  ParsePresent(input, index, all_presents);
  ParsePresent(input, index, all_presents);

  Areas areas;
  while (index < input.size() - 1) {
    ParseArea(input, index, areas);
  }

  Eigen::Array<u32, 6, 1> present_areas;
  for (u32 i = 0; i < all_presents.size(); i++) {
    present_areas(i) = all_presents[i].sum();
  }

  // Part 1
  u64 part1 = 0;
  for (const auto& [w, h, n] : areas) {
    if (h * w >= (present_areas * n).sum()) part1++;
  }

  // Part 2
  u64 part2 = 0;

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 12>::PartOne() -> std::string { return "481"; }

template<> auto advent<2025, 12>::PartTwo() -> std::string { return "0"; }
