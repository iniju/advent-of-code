#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Pattern = absl::string_view;
using Patterns = std::vector<Pattern>;
using Cache = absl::flat_hash_map<Pattern, u64>;

u64 Formable(const Pattern &pattern, const Patterns &towels, Cache &cache) {
  if (cache.contains(pattern)) return cache.at(pattern);
  u64 solutions = 0;
  for (const auto &towel : towels) {
    if (!pattern.starts_with(towel)) continue;
    if (pattern.size() == towel.size()) {
      solutions++;
      continue;
    }
    solutions += Formable(pattern.substr(towel.size()), towels, cache);
  }
  if (solutions > 0) cache[pattern] = solutions;
  return solutions;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 19>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> parts = absl::StrSplit(input, "\n\n", absl::SkipWhitespace());
  Patterns towels = absl::StrSplit(parts.at(0), ", ", absl::SkipWhitespace());
  Patterns patterns = absl::StrSplit(parts.at(1), "\n", absl::SkipWhitespace());

  // Part 1 & Part 2
  u64 part1 = 0, part2 = 0;
  Cache cache;
  for (u32 p = 0; p < patterns.size(); p++) {
    const Pattern &pattern = patterns.at(p);
    u64 solutions = Formable(pattern, towels, cache);
    if (solutions > 0) part1++;
    part2 += solutions;
  }

  return aoc::result(part1, part2);
}