#include <aoc.hpp>

namespace {

using Operand = u64;
using Operands = std::vector<Operand>;
using Equation = std::pair<Operand, Operands>;
using Equations = std::vector<Equation>;

bool IsValid(const Operand goal, const Operands &operands, const u16 operand_i, const bool use_concat) {
  const auto operand = operands[operand_i];
  if (operand_i == 0) return goal == operand;

  const auto [q, r] = std::div(static_cast<i64>(goal), static_cast<i64>(operand));
  if (r == 0 && IsValid(q, operands, operand_i - 1, use_concat)) return true;
  if (use_concat) {
    const auto tens = aoc::kTenPowers[aoc::util::NumDigits(operand)];
    if ((goal - operand) % tens == 0 && IsValid(goal / tens, operands, operand_i - 1, use_concat)) return true;
  }
  if (IsValid(goal - operand, operands, operand_i - 1, use_concat)) return true;
  return false;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 7>::solve() -> Result {
  Equations equations;
  for (auto line : input | std::views::split('\n')) {
    Equation equation;
    auto line_split_view = line | std::views::split(':');
    auto parts_it = line_split_view.begin();
    const auto start = std::ranges::data(*parts_it);
    auto [_, ec] = std::from_chars(start, start + std::ranges::size(*parts_it), equation.first);
    CHECK(ec == std::errc()) << "Failed to parse first number";
    std::advance(parts_it, 1);
    // Use a new subrange, skipping the first character, which is the space following the colon.
    aoc::util::FasterScanList(std::ranges::subrange((*parts_it).begin() + 1, (*parts_it).end()), equation.second);
    equations.emplace_back(equation.first, equation.second);
  }

  // Part 1 & Part 2
  Operand part1 = 0, part2 = 0;
  for (const auto & [goal, operands] : equations) {
    if (IsValid(goal, operands, operands.size() - 1, false)) {
      part1 += goal;
    } else {
      if (IsValid(goal, operands, operands.size() - 1, true)) part2 += goal;
    }
  }
  part2 += part1;

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 7>::PartOne() -> std::string { return "14711933466277"; }

template<> auto advent<2024, 7>::PartTwo() -> std::string { return "286580387663654"; }
