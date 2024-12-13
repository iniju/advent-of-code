#include <aoc.hpp>

#include <fmt/format.h>
#include "re2/re2.h"

namespace {

RE2 InputPattern(R"(Button A: X\+(\d+), Y\+(\d+)\nButton B: X\+(\d+), Y\+(\d+)\nPrize: X=(\d+), Y=(\d+))");
using Arcade = std::tuple<i64, i64, i64, i64, i64, i64>;
using Arcades = std::vector<Arcade>;

bool SolveArcade(const Arcade &arcade, i64 &tokens, bool is_part2) {
  i64 ax, ay, bx, by, gx, gy;
  std::tie(ax, ay, bx, by, gx, gy) = arcade;
  if (is_part2) {
    gx += 10'000'000'000'000;
    gy += 10'000'000'000'000;
  }
  i64 a_nom = gy * bx - gx * by;
  i64 a_denom = ay * bx - ax * by;
  if (a_denom == 0 || a_nom % a_denom != 0) return false;
  i64 a = a_nom / a_denom;
  i64 b_nom = gx - a * ax;
  if (b_nom % bx != 0) return false;
  i64 b = b_nom / bx;
  if (!is_part2 && (a > 100 || b > 100)) return false;
  tokens = 3 * a + b;
  return true;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 13>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> parts = absl::StrSplit(input, "\n\n", absl::SkipWhitespace());
  Arcades arcades;
  absl::c_transform(parts, std::back_inserter(arcades), [](absl::string_view part) {
    i64 ax, ay, bx, by, gx, gy;
    CHECK(RE2::FullMatch(part, InputPattern, &ax, &ay, &bx, &by, &gx, &gy)) << "Could parse '" << part << "'";
    return std::make_tuple(ax, ay, bx, by, gx, gy);
  });

  // Part 1 & Part 2
  u64 part1 = 0, part2 = 0;
  i64 tokens;
  for (const auto &arcade : arcades) {
    if (SolveArcade(arcade, tokens, false)) part1 += tokens;
    if (SolveArcade(arcade, tokens, true)) part2 += tokens;
  }

  return aoc::result(part1, part2);
}