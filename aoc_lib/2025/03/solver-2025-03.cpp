#include <aoc.h>

#include <fmt/format.h>

namespace {

using Bank = std::string_view;
using Banks = std::vector<Bank>;

u64 ScanBank(std::string_view bank, u32 bak_size, u32 needed) {
  u64 joltage = 0;
  auto from = bank.begin();
  auto to = from + bak_size - needed + 1;
  for (u32 battery = 0; battery < needed; battery++) {
    auto max_jolt_it = std::max_element(from, to);
    from = max_jolt_it + 1;
    to += 1;
    joltage = joltage * 10 + (*max_jolt_it - '0');
  }
  return joltage;
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 03>::solve() -> Result {
  Banks banks = input | std::views::split('\n') | std::views::transform([](auto rng) {
                  return std::string_view(&*std::ranges::begin(rng), std::ranges::distance(rng));
                }) |
                std::ranges::to<Banks>();
  i32 bank_size = static_cast<i32>(banks[0].length());

  // Part 1 & 2
  u64 part1 = 0, part2 = 0;
  for (auto bank : banks) {
    part1 += ScanBank(bank, bank_size, 2);
    part2 += ScanBank(bank, bank_size, 12);
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 03>::PartOne() -> std::string { return "17074"; }

template<> auto advent<2025, 03>::PartTwo() -> std::string { return "169512729575727"; }
