#include <aoc.hpp>

#include <ostream>
#include <absl/container/flat_hash_map.h>

namespace {

using LineSymbolsByColumn = absl::flat_hash_map<int, char>;
using SymbolsByLine = absl::flat_hash_map<int, LineSymbolsByColumn>;
struct Number {
  int start;
  int end;
  u32 num;
};
using NumbersByLine = absl::flat_hash_map<int, std::vector<Number>>;
using Gears = absl::flat_hash_map<std::pair<int, int>, std::vector<u32>>;

auto is_dot = [](char ch) {
  return ch == '.';
};
auto is_number = [](char ch) {
  return ch >= '0' && ch <= '9';
};
auto is_symbol = [](char ch) {
  return ch != '.' && (ch < '0' || ch > '9');
};

template<typename InputIt>
int ParseNum(InputIt *first, InputIt last) {
  int num = **first - '0';
  while (++(*first) != last && is_number(**first)) {
    num = num * 10 + (**first - '0');
  }
  (*first)--;
  return num;
}

}  // namespace

template<>
auto advent<2023, 3>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = aoc::util::TokenizeInput<absl::string_view>(
      input,
      [](absl::string_view line) { return line; });

  SymbolsByLine symbols{};
  Gears gears{};
  NumbersByLine numbers;
  for (int line_index = 0; line_index < lines.size(); line_index++) {
    auto line = lines[line_index];
    // Skip until next not dot
    auto it = absl::c_find_if_not(line, is_dot);
    while (it != line.end()) {
      if (is_symbol(*it)) {
        // Found symbol
        char symbol = *it;
        // fmt::print("{} @ {},{}\n", *it, line_index, it - line.begin());
        symbols[line_index][(int) (it - line.begin())] = symbol;
        // Store the gears also by coordinates.
        if (symbol == '*') gears[{line_index, (int) (it - line.begin())}] = {};
      } else {
        // Found start of number, retrieve whole number.
        Number number{};
        number.start = (int) (it - line.begin());
        number.num = ParseNum(&it, line.end());
        number.end = (int) (it - line.begin());
        //  fmt::print("{} @ {},[{}..{}]\n", number.num, line_index, number.start, number.end);
        numbers[line_index].push_back(number);
      }
      // Skip until next not dot
      it = std::find_if_not(++it, line.end(), is_dot);
    }
  }

  // Part 1 & Part 2
  u64 part1 = 0;
  for (const auto &[line, numbers_in_line] : numbers) {
    for (const auto &num : numbers_in_line) {
      bool is_part = false;
      for (int y = line - 1; y <= line + 1; y++) {
        for (int x = num.start - 1; x <= num.end + 1; x++) {
          if (!symbols.contains(y)) continue;
          if (symbols.at(y).contains(x)) {
            is_part = true;
            // Normally would break here, but better to scan all adjacent symbols for Part 2.
            // break;
            if (symbols.at(y).at(x) == '*') {
              gears.at({y, x}).push_back(num.num);
            }
          }
        }
        // Normally would break here, but better to scan all adjacent symbols for Part 2.
      }
      if (is_part) {
        part1 += num.num;
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  for (const auto &[xy, num_adjacent_to_gear] : gears) {
    if (num_adjacent_to_gear.size() == 2) {
      part2 += num_adjacent_to_gear.at(0) * num_adjacent_to_gear.at(1);
    }
  }

  return aoc::result(part1, part2);
}