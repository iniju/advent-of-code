#include <aoc.hpp>

#include <fast_float/fast_float.h>
#include <re2/re2.h>

namespace {

RE2 InstructionPattern(R"(((?:mul\([\d,]+\)|do\(\)|don't\(\))))");

std::tuple<i64, i64> ParseInstructions(absl::string_view input) {
  i64 part1 = 0;
  i64 part2 = 0;
  absl::string_view instruction;
  bool skip = false;
  while (RE2::FindAndConsume(&input, InstructionPattern, &instruction)) {
    char ch = instruction.at(2);
    if (ch != 'l') {  // Handle do()s and don't()s.
      skip = ch == 'n';
      continue;
    }
    i64 x, y;
    auto line_end = instruction.data() + instruction.size();
    // Skip the "mul(" part.
    auto ret = fast_float::from_chars(instruction.data() + 4, line_end, x);
    // Skip the comma.
    fast_float::from_chars(ret.ptr + 1, line_end, y);
    i64 value = x * y;
    part1 += value;
    if (!skip) part2 += value;
  }
  return std::make_tuple(part1, part2);
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 03>::solve() -> Result {
  // Part 1 & 2
  auto [part1, part2] = ParseInstructions(input);

  return aoc::result(part1, part2);
}