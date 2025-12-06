#include <aoc.h>

#include <fmt/format.h>

namespace {

enum Op : char {
  ADDITION = '+',
  MULTIPLICATION = '*',
};

using NumRow = std::vector<u64>;
using Sheet = std::vector<NumRow>;
using OpList = std::vector<Op>;

}  // namespace

namespace fmt {

template<>
struct formatter<Op> : formatter<char> {
  auto format(const Op op, format_context& ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(op), ctx);
  }
};

}  // namespace fmt

template<>
auto advent<2025, 06>::solve() -> Result {
  Sheet sheet;
  OpList ops;
  std::vector<std::string> lines = absl::StrSplit(input, "\n");
  for (auto line : lines) {
    if (line[0] == '*' || line[0] == '+') {
      for (auto c : line) {
        if (c == ' ') continue;
        ops.push_back(c == '+' ? Op::ADDITION : Op::MULTIPLICATION);
      }
      break;
    }
    sheet.emplace_back();
    aoc::util::FastScanList(line, sheet.back());
  }

  // Part 1
  u64 part1 = 0;
  for (u32 j = 0; j < ops.size(); j++) {
    auto op = ops[j];
    u64 sum = op == ADDITION ? 0 : 1;
    for (u32 i = 0; i < sheet.size(); i++) {
      switch (op) {
        case ADDITION: sum += sheet[i][j]; break;
        case MULTIPLICATION: sum *= sheet[i][j]; break;
      }
    }
    part1 += sum;
  }

  // Part 2
  u64 part2 = 0;
  u64 j_from = 0, j_to;
  u32 num_rows = lines.size() - 1;
  do {
    j_to = lines.back().find_first_of("+*", j_from + 1);
    j_to = (j_to == std::string::npos) ? lines.back().size() - 1 : j_to - 2;
    Op op;
    u64 sum;
    if (lines[num_rows][j_from] == '+') {
      op = Op::ADDITION;
      sum = 0;
    } else {
      op = Op::MULTIPLICATION;
      sum = 1;
    }
    for (u32 j = j_from; j <= j_to; j++) {
      u64 number = 0;
      for (u32 i = 0; i < num_rows; i++) {
        if (lines[i][j] == ' ') continue;
        number = number * 10 + lines[i][j] - '0';
      }
      switch (op) {
        case ADDITION: sum += number; break;
        case MULTIPLICATION: sum *= number; break;
      }
    }
    part2 += sum;
    j_from = j_to + 2;

  } while (j_from < lines.back().size());

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 06>::PartOne() -> std::string { return "4693419406682"; }

template<> auto advent<2025, 06>::PartTwo() -> std::string { return "9029931401920"; }
