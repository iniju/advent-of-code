#include <aoc.hpp>

#include <re2/re2.h>

namespace {

RE2 InstructionPattern(R"(((?:mul\([\d,]+\)|do\(\)|don't\(\))))");

void ParseInstructions(absl::string_view input, i64& part1, i64 & part2) {
  absl::string_view instruction;
  bool skip = false;
  while (RE2::FindAndConsume(&input, InstructionPattern, &instruction)) {
    if (const char ch = instruction.at(2); ch != 'l') {  // Handle do()s and don't()s.
      skip = ch == 'n';
      continue;
    }
    i64 x, y;
    const auto operands_start = std::ranges::data(instruction);
    const auto line_end = operands_start + std::ranges::size(instruction);
    // Skip the "mul(" part.
    auto [ptr, ec] = fast_float::from_chars(operands_start + 4, line_end, x);
    // Skip the comma.
    fast_float::from_chars(ptr + 1, line_end, y);
    const i64 value = x * y;
    part1 += value;
    if (!skip) part2 += value;
  }
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 03>::solve() -> Result {
  // Part 1 & 2
  i64 part1 = 0, part2 = 0;
  ParseInstructions(input, part1, part2);

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 3>::PartOne() -> std::string { return "187833789"; }

template<> auto advent<2024, 3>::PartTwo() -> std::string { return "94455185"; }
