#include <aoc.hpp>
#include <absl/container/flat_hash_map.h>

namespace {
absl::flat_hash_map<absl::string_view, int> word_value = {
    {"0", 0}, {"zero", 0},
    {"1", 1}, {"one", 1},
    {"2", 2}, {"two", 2},
    {"3", 3}, {"three", 3},
    {"4", 4}, {"four", 4},
    {"5", 5}, {"five", 5},
    {"6", 6}, {"six", 6},
    {"7", 7}, {"seven", 7},
    {"8", 8}, {"eight", 8},
    {"9", 9}, {"nine", 9},
};
std::vector<absl::string_view> digits =
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
std::vector<absl::string_view> word_digits = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "zero", "one", "two", "three", "four",
    "five", "six", "seven", "eight", "nine"
};

int findDigits(absl::string_view input, const std::vector<absl::string_view> &keywords) {
  u64 first_pos = std::string::npos;
  int first = 0;
  i64 last_pos = -1;
  int last = 0;
  i64 match;
  for (auto k : keywords) {
    match = (i64) input.find(k);
    if (first_pos > match) {
      first_pos = match;
      first = word_value[k];
    }
    match = (i64) input.rfind(k);
    if (match != std::string::npos && last_pos < match) {
      last_pos = match;
      last = word_value[k];
    }
  }
  return 10 * first + last;
}

}  // namespace detail

template<>
auto advent2023::day01() -> result {
  std::string input = aoc::util::GetInput(year, 1);
  std::vector<absl::string_view> lines = aoc::util::TokenizeInput<absl::string_view>(
      input,
      [](absl::string_view line) { return line; });

  // Part 1
  std::vector<int> calibrations1;
  absl::c_transform(lines, std::back_inserter(calibrations1), [](auto line) {
    return findDigits(line, digits);
  });
  u32 part1 = absl::c_accumulate(calibrations1, 0, [](int a, int b) {
    return a + b;
  });

  // Part 2
  std::vector<int> calibrations2;
  absl::c_transform(lines, std::back_inserter(calibrations2), [](auto line) {
    return findDigits(line, word_digits);
  });
  u32 part2 = absl::c_accumulate(calibrations2, 0, [](int a, int b) {
    return a + b;
  });

  return aoc::result(part1, part2);
}