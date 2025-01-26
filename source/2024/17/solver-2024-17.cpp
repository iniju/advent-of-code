#include <aoc.hpp>

#include <fmt/format.h>
#include <re2/re2.h>

namespace {

RE2 RegisterPattern(R"(Register .: (\d+))");
enum class Op : u8 {
  ADV = 0,
  BXL = 1,
  BST = 2,
  JNZ = 3,
  BXC = 4,
  OUT = 5,
  BDV = 6,
  CDV = 7,
};
using Registers = std::vector<u64>;
using Output = std::vector<u8>;

u64 GetComboValue(u8 operand, const Registers &regs) {
  CHECK(operand < 7) << "Combo operand can't be 7.";
  return operand < 4 ? operand : regs.at(operand - 4);
}
void Execute(Op op, u8 operand, Registers &regs, Output &output, u16 &instruction) {
  switch (op) {
    case Op::ADV: regs[0] = regs[0] / (1 << GetComboValue(operand, regs));
      break;
    case Op::BXL: regs[1] ^= operand;
      break;
    case Op::BST: regs[1] = GetComboValue(operand, regs) % 8;
      break;
    case Op::JNZ:
      if (regs[0] > 0) {
        instruction = operand;
        return;
      }
      break;
    case Op::BXC: regs[1] ^= regs[2];
      break;
    case Op::OUT: output.push_back(GetComboValue(operand, regs) % 8);
      break;
    case Op::BDV: regs[1] = regs[0] / (1 << GetComboValue(operand, regs));
      break;
    case Op::CDV: regs[2] = regs[0] / (1 << GetComboValue(operand, regs));
      break;
  }
  instruction += 2;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 17>::solve() -> Result {
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  CHECK(lines.size() == 4) << "Unexpected number of lines: " << lines.size();
  std::vector<u64> regs(3, 0);
  for (i16 i = 0; i < 3; i++) {
    CHECK(RE2::FullMatch(lines.at(i), RegisterPattern, &regs[i]))
            << "Couldn't parse register initial value from '" << lines.at(i) << "'.";
  }
  auto program_str = lines[3].substr(lines[3].find(' ') + 1);
  std::vector<u8> program;
  aoc::util::ScanList(program_str, program, ",");

  // Part 1
  u16 instruction = 0;
  Output output;
  while (instruction < program.size()) {
    Execute(static_cast<Op>(program.at(instruction)), program.at(instruction + 1), regs, output, instruction);
  }
  std::string part1 = absl::StrJoin(output, ",");

  // Part 2
  Registers reg_a_values{0};
  Registers next_reg_a_values;
  for (i32 i = static_cast<i32>(program.size()) - 1; i >= 0; i--) {
    for (auto reg_a : reg_a_values) {
      for (u64 d = 0; d < 8; d++) {
        regs[0] = reg_a * 8 + d;
        regs[1] = 0;
        regs[2] = 0;
        instruction = 0;
        output.clear();
        while (instruction < program.size()) {
          Execute(static_cast<Op>(program.at(instruction)), program.at(instruction + 1), regs, output, instruction);
        }
        if (std::equal(output.begin(), output.end(), program.begin() + i, program.end())) {
          next_reg_a_values.push_back(reg_a * 8 + d);
        }
      }
    }
    std::swap(reg_a_values, next_reg_a_values);
    next_reg_a_values.clear();
  }
  u64 part2 = *absl::c_min_element(reg_a_values);

  return aoc::result(part1, part2);
}