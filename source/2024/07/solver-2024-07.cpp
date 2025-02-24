#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Operands = std::vector<u64>;
using Equation = std::pair<u64, Operands>;
using Equations = std::vector<Equation>;

enum class Op : u16 {
  ADD, MUL, CONCAT
};

u64 ApplyOp(u64 left, u64 right, Op op) {
  switch (op) {
    case Op::ADD: return left + right;
    case Op::MUL: return left * right;
    case Op::CONCAT: {
      u32 tens = aoc::util::NumDigits(right);
      return left * aoc::kTenPowers[tens] + right;
    }
  }
}

std::pair<bool, u64> IsValid(u64 goal, u64 cur, const Operands &operands, u16 operand_i, bool use_concat) {
  if (cur > goal) return std::make_pair(false, 0);
  if (operand_i == operands.size() - 1) {
    u64 result = ApplyOp(cur, operands.at(operand_i), Op::ADD);
    if (goal == result) return std::make_pair(true, result);
    result = ApplyOp(cur, operands.at(operand_i), Op::MUL);
    if (goal == result) return std::make_pair(true, result);
    if (use_concat) {
      result = ApplyOp(cur, operands.at(operand_i), Op::CONCAT);
      if (goal == result) return std::make_pair(true, result);
    }
    return std::make_pair(false, 0);
  }
  auto result = IsValid(goal, ApplyOp(cur, operands.at(operand_i), Op::ADD), operands, operand_i + 1, use_concat);
  if (result.first)
    return
        result;
  result = IsValid(goal, ApplyOp(cur, operands.at(operand_i), Op::MUL), operands, operand_i + 1, use_concat);
  if (result.first)
    return
        result;
  if (use_concat) {
    result = IsValid(goal, ApplyOp(cur, operands.at(operand_i), Op::CONCAT), operands, operand_i + 1, use_concat);
    if (result.first)
      return
          result;
  }
  return std::make_pair(false, 0);
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 7>::solve() -> Result {
  Equations equations = aoc::util::TokenizeInput<Equation>(input, [](auto line) {
    std::vector<std::string> parts = absl::StrSplit(line, ": ", absl::SkipWhitespace());
    CHECK(parts.size() == 2) << "Unexpected format in '" << line << "'.";
    Equation equation;
    auto result = scn::scan<u64>(parts[0], "{}");
    CHECK(result) << "Unable to parse result from '" << parts[0] << "'.";
    equation.first = result->value();
    aoc::util::ScanList(parts[1], equation.second);
    return equation;
  });

  // Part 1
  u64 part1 = 0;
  Equations invalid_equations;
  for (const auto &equation : equations) {
    auto [valid, result] = IsValid(equation.first, equation.second.at(0), equation.second, 1, false);
    if (valid) part1 += result; else invalid_equations.push_back(equation);
  }

  // Part 2
  u64 part2 = part1;
  for (const auto &equation : invalid_equations) {
    auto [valid, result] = IsValid(equation.first, equation.second.at(0), equation.second, 1, true);
    if (valid) part2 += result;
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 7>::PartOne() -> std::string { return "14711933466277"; }

template<> auto advent<2024, 7>::PartTwo() -> std::string { return "286580387663654"; }
