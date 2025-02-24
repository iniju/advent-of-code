#include <aoc.hpp>

#include <absl/container/btree_set.h>
#include <fmt/format.h>
#include <re2/re2.h>

namespace {

RE2 InputPattern(R"(([URDL]) (\d+) \(#([0-9a-f]+)\))");
struct Instruction {
  aoc::Dir dir{};
  i64 move{};
};

aoc::Dir ToDir(char c) {
  switch (c) {
    case 'U': return aoc::Dir::N;
    case 'R': return aoc::Dir::E;
    case 'D': return aoc::Dir::S;
    case 'L': return aoc::Dir::W;
    default : CHECK(false) << "Unexpected direction '" << c << "'.";
  }
}

Instruction FromColor(absl::string_view color) {
  Instruction ins;
  switch (color.back()) {
    case '0':
      ins.dir = aoc::Dir::E;
      break;
    case '1':
      ins.dir = aoc::Dir::S;
      break;
    case '2':
      ins.dir = aoc::Dir::W;
      break;
    case '3':
      ins.dir = aoc::Dir::N;
      break;
    default: CHECK(false) << "Unexpected last digit '" << color.back() << "'.";
  }
  auto hex_str = fmt::format("0x{}", color.substr(0, color.size() - 1));
  CHECK(scn::scan(hex_str, "{:x}", ins.move)) << "Couldn't parse hex '" << hex_str << "'.";
  return ins;
}

u64 ShoeLace(const std::vector<Instruction> &instructions) {
  aoc::Pos x1{0, 0};
  i64 result{0};
  i64 perimeter{0};
  for (auto ins : instructions) {
    aoc::Pos x2 = x1 + aoc::MoveDir(ins.dir) * ins.move;
    result += static_cast<i64>(x1.i) * static_cast<i64>(x2.j) - static_cast<i64>(x1.j) * static_cast<i64>(x2.i);
    perimeter += static_cast<i64>(ins.move);
    x1 = x2;
  }
  return (abs(result) + perimeter) / 2ll + 1ll;
}

}  // namespace

namespace fmt {

template<>
struct formatter<Instruction> : formatter<string_view> {
  auto format(const Instruction &i, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("{} {}", i.dir, i.move), ctx);
  }
};

}  // namespace fmt

template<>
auto advent<2023, 18>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> colors;
  std::vector<Instruction>
      instructions1 = aoc::util::TokenizeInput<Instruction>(input, [&colors](absl::string_view line) {
    Instruction ins;
    char ins_char;
    absl::string_view color;
    CHECK(RE2::FullMatch(line, InputPattern, &ins_char, &ins.move, &color)) << "Couldn't parse '" << line << "'.";
    colors.push_back(color);
    ins.dir = ToDir(ins_char);
    return ins;
  });

  // Part 1
  u64 part1 = ShoeLace(instructions1);

  // Part 2
  std::vector<Instruction> instructions2;
  absl::c_transform(colors, std::back_inserter(instructions2), [](absl::string_view color) {
    return FromColor(color);
  });
  u64 part2 = ShoeLace(instructions2);

  return aoc::result(part1, part2);
}