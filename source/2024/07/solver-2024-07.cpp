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
auto advent<2024, 07>::solve() -> Result {
  std::string input = GetInput();
  auto start_0 = std::chrono::high_resolution_clock::now();

  Equations equations = aoc::util::TokenizeInput<Equation>(input, [](auto line) {
    std::vector<std::string> parts = absl::StrSplit(line, ": ", absl::SkipWhitespace());
    CHECK(parts.size() == 2) << "Unexpected format in '" << line << "'.";
    Equation equation;
    CHECK(scn::scan(parts[0], "{}", equation.first)) << "Unable to parse result from '" << parts[0] << "'.";
    CHECK(scn::scan_list(parts[1], equation.second)) << "Unable to parse operands from '" << parts[1] << "'.";
    return equation;
  });

  // Part 1
  auto start_1 = std::chrono::high_resolution_clock::now();
  u64 part1 = 0;
  Equations invalid_equations;
  for (const auto &equation : equations) {
    auto [valid, result] = IsValid(equation.first, equation.second.at(0), equation.second, 1, false);
    if (valid) part1 += result; else invalid_equations.push_back(equation);
  }

  // Part 2
  auto start_2 = std::chrono::high_resolution_clock::now();
  u64 part2 = part1;
  for (const auto &equation : invalid_equations) {
    auto [valid, result] = IsValid(equation.first, equation.second.at(0), equation.second, 1, true);
    if (valid) part2 += result;
  }
  auto end = std::chrono::high_resolution_clock::now();
  fmt::print("Parsing: {:0.5f} sec\nPart 1: {:0.5f} sec\nPart 2: {:0.5f} sec\n",
             (long double) std::chrono::duration_cast<std::chrono::nanoseconds>(start_1 - start_0).count() / 1.0e9l,
             (long double) std::chrono::duration_cast<std::chrono::nanoseconds>(start_2 - start_1).count() / 1.0e9l,
             (long double) std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_2).count() / 1.0e9l);

  return aoc::result(part1, part2);
}