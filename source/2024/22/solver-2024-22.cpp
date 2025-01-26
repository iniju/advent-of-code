#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Secret = u32;
using DiffSeq = u32;

inline Secret GetNextSecret(Secret secret) {
  secret = (secret ^ (secret << 6)) & 0xFFFFFF;
  secret = (secret ^ (secret >> 5)) & 0xFFFFFF;
  return (secret ^ (secret << 11)) & 0xFFFFFF;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 22>::solve() -> Result {
  std::vector<Secret> initial_secrets = aoc::util::TokenizeInput<Secret>(input, [](auto line) {
    Secret val;
    CHECK(std::from_chars(line.data(), line.data() + line.size(), val).ec == std::errc{})
            << "Unable to parse '" << line << "'.";
    return val;
  });

  // Part 1 & Part 2
  u64 part1 = 0;
  DiffSeq diff_seq{0};
  std::vector<u32> seen(0xFFFFF, std::numeric_limits<u32>::max());
  std::vector<u32> bought(0xFFFFF, 0);
  for (u32 buyer = 0; buyer < initial_secrets.size(); buyer++) {
    Secret secret = initial_secrets.at(buyer);
    auto old_price = secret % 10;
    for (u16 i = 0; i < 2000; i++) {
      secret = GetNextSecret(secret);
      auto price = secret % 10;
      auto price_diff = price - old_price + 9;
      old_price = price;
      diff_seq <<= 5;
      diff_seq |= price_diff;
      if (i < 3) continue; else diff_seq &= 0xFFFFF;

      if (seen.at(diff_seq) == buyer) continue;
      seen[diff_seq] = buyer;
      bought[diff_seq] += price;
    }
    part1 += secret;
  }
  u64 part2 = *absl::c_max_element(bought);

  return aoc::result(part1, part2);
}