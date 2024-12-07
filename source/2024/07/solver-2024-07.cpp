#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Operands = std::vector<u64>;
using Equation = std::pair<u64, Operands>;
using Equations = std::vector<Equation>;

enum class Op : u16 {
  ADD, MUL, CONCAT
};
using OpSet = std::vector<Op>;

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
u64 ApplyOpSet(const Operands& operands, const OpSet& operators) {
  u64 result = operands.at(0);
  for (u64 i = 0; i < operators.size(); i++) {
    result = ApplyOp(result, operands.at(i + 1), operators.at(i));
  }
  return result;
}
bool NextCombination(OpSet& op_set, u16 op_num) {
  u32 op_size = op_set.size();
  for (u32 i = 0; i < op_size; i++) {
    u16 op_ord = aoc::ToUnderlying(op_set[i]);
    if (op_ord < op_num - 1) {
      op_set[i] = static_cast<Op>(op_ord + 1);
      return true;
    }
    if (i == op_size - 1) return false;
    op_set[i] = Op::ADD;
  }
  fmt::print("Should never reach here.\n");
  return false;
}

}  // namespace

namespace fmt {

template<>
struct formatter<Op> : formatter<char> {
  auto format(Op op, format_context &ctx) const {
    switch (op) {
      case Op::ADD: return formatter<char>::format('+', ctx);
      case Op::MUL: return formatter<char>::format('*', ctx);
      case Op::CONCAT: return formatter<char>::format('|', ctx);
    }
  }
};

}  // namespace fmt

template<>
auto advent<2024, 07>::solve() -> Result {
  std::string input = GetInput();
  Equations equations = aoc::util::TokenizeInput<Equation>(input, [](auto line) {
    std::vector<std::string> parts = absl::StrSplit(line, ": ", absl::SkipWhitespace());
    CHECK(parts.size() == 2) << "Unexpected format in '" << line << "'.";
    Equation equation;
    CHECK(scn::scan(parts[0], "{}", equation.first)) << "Unable to parse result from '" << parts[0] << "'.";
    CHECK(scn::scan_list(parts[1], equation.second)) << "Unable to parse operands from '" << parts[1] << "'.";
    return equation;
  });

  // Part 1
  u64 part1 = 0;
  Equations invalid_equations;
  for (const auto& equation : equations) {
    u64 op_num = equation.second.size() - 1;
    OpSet op_comb(op_num, Op::ADD);
    bool valid = false;
    do {
      u64 result = ApplyOpSet(equation.second, op_comb);
      if (result == equation.first) {
        part1 += result;
        valid = true;
        break;
      }
    } while (NextCombination(op_comb, 2));
    if (!valid) invalid_equations.push_back(equation);
  }

  // Part 2
  u64 part2 = part1;
  for (const auto& equation : invalid_equations) {
    u64 op_num = equation.second.size() - 1;
    OpSet op_comb(op_num, Op::ADD);
    op_comb[0] = Op::CONCAT;
    do {
      // Skip if we don't use any CONCAT, since we already checked all these and they failed for this case.
      if (absl::c_find(op_comb, Op::CONCAT) == op_comb.end()) continue;
      u64 result = ApplyOpSet(equation.second, op_comb);
      if (result == equation.first) {
        part2 += result;
        break;
      }
    } while (NextCombination(op_comb, 3));
  }

  return aoc::result(part1, part2);
}