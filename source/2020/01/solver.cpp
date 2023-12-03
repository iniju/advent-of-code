#include <aoc.hpp>

namespace detail {

auto FindSumGoal(std::vector<u32>::const_iterator begin,
                 std::vector<u32>::const_iterator end, u32 goal)
-> std::optional<i64> {
  for (auto i = begin; i != end - 1; i++) {
    for (auto j = i + 1; j != end; j++) {
      u32 sum = *i + *j;
      if (sum == goal) {
        return *i * *j;
      } else if (sum > goal) {
        break;
      }
    }
  }
  return std::nullopt;
}

}  // namespace detail

template<>
auto advent2020::day01() -> result {
  std::string input = aoc::util::GetInput(year, 1);
  std::vector<u32> expenses = aoc::util::TokenizeInput<u32>(
      input,
      [](absl::string_view s) { return scn::scan_value<u32>(s).value(); });
  absl::c_sort(expenses);

  auto p1 =
      detail::FindSumGoal(expenses.cbegin(), expenses.cend(), 2020).value();
  u32 p2 = 0;
  for (auto i = expenses.cbegin(); i != expenses.cend() - 2; i++) {
    u32 target = 2020 - *i;
    auto answer = detail::FindSumGoal(i + 1, expenses.cend(), target);
    if (answer.has_value()) {
      p2 = *i * answer.value();
      break;
    }
  }
  return aoc::result(p1, p2);
}