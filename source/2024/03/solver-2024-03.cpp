#include <aoc.hpp>

#include <fmt/format.h>
#include <re2/re2.h>

namespace {

RE2 InstructionPattern(R"(mul\((\d+),(\d+)\))");
RE2 DisabledSectionPattern(R"((?s)don't\(\).*?do\(\))");
RE2 FinalDisabledSectionPattern(R"((?s)don't\(\).*$)");

i64 ParseInstructions(absl::string_view input) {
  i64 result = 0;
  i64 x, y;
  while (RE2::FindAndConsume(&input, InstructionPattern, &x, &y)) {
    result += x * y;
  }
  return result;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 03>::solve() -> Result {
  // Part 1
  u64 part1 = ParseInstructions(input);

  // Part 2
  std::string fixed_input(input);
  while(RE2::GlobalReplace(&fixed_input, DisabledSectionPattern, "do()"));
  CHECK(RE2::Replace(&fixed_input, FinalDisabledSectionPattern, ""));
  u64 part2 = ParseInstructions(fixed_input);

  return aoc::result(part1, part2);
}