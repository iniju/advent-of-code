#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Stone = u64;
using SplitStones = std::pair<Stone, Stone>;
using Stones = std::vector<Stone>;
using StoneTimes = std::pair<Stone, u32>;
using Cache = absl::flat_hash_map<StoneTimes, u64>;

SplitStones SplitStone(Stone stone, u32 digits) {
  digits /= 2;
  u64 left = stone / aoc::kTenPowers[digits];
  return {left, stone - left * aoc::kTenPowers[digits]};
}

u64 Blink(Stone stone, u32 times, Cache &cache) {
  if (times == 0) return 1;
  u64 result;
  StoneTimes key{stone, times};
  if (cache.contains(key)) return cache.at(key);
  if (stone == 0) {
    result = Blink(1, times - 1, cache);
    cache[key] = result;
    return result;
  }
  u32 digits = aoc::util::NumDigits(stone);
  if ((digits & 1) == 1) {
    result = Blink(stone * 2024, times - 1, cache);
    cache[key] = result;
    return result;
  }
  SplitStones pair = SplitStone(stone, digits);
  result = Blink(pair.first, times - 1, cache) + Blink(pair.second, times - 1, cache);
  cache[key] = result;
  return result;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 11>::solve() -> Result {
  std::string input = GetInput();
  Stones stones = aoc::util::TokenizeInput<Stone>(input, [](auto token) {
    auto result = scn::scan<u64>(token, "{}");
    CHECK(result) << "Couldn't parse '" << token << "'.";
    return result->value();
  }, " ");

  // Part 1
  u64 part1 = 0;
  Cache cache;
  for (auto stone : stones) {
    part1 += Blink(stone, 25, cache);
  }

  // Part 2
  u64 part2 = 0;
  for (auto stone : stones) {
    part2 += Blink(stone, 75, cache);
  }

  return aoc::result(part1, part2);
}