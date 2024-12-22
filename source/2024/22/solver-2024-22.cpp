#include <aoc.hpp>

#include <fmt/format.h>

namespace {

using Secret = u32;
using Price = i8;

inline Secret GetNextSecret(Secret secret) {
  Secret result = ((secret << 6) ^ secret) & 0xFFFFFF;
  result = ((result >> 5) ^ result) & 0xFFFFFF;
  return ((result << 11) ^ result) & 0xFFFFFF;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 22>::solve() -> Result {
  std::string input = GetInput();
  std::vector<Secret> initial_secrets = aoc::util::TokenizeInput<Secret>(input, [](auto line) {
    Secret val;
    CHECK(scn::scan(line, "{}", val)) << "Unable to parse '" << line << "'.";
    return val;
  });

  // Part 1
  u64 part1 = 0;
  std::deque<Price> diff_seq;
  absl::flat_hash_set<std::deque<Price>> all_diff_seqs;
  std::deque<absl::flat_hash_map<std::deque<Price>, Price>> seq_to_price;
  for (Secret initial_secret : initial_secrets) {
    Secret secret = initial_secret;
    auto old_price = static_cast<Price>(secret % 10);
    diff_seq.clear();
    seq_to_price.emplace_back();
    for (u16 i = 0; i < 2000; i++) {
      secret = GetNextSecret(secret);
      auto price = static_cast<Price>(secret % 10);
      auto price_diff = static_cast<i8>(price - old_price);
      old_price = price;
      diff_seq.push_back(price_diff);
      if (i < 3) continue; else if (i > 3) diff_seq.pop_front();
      if (seq_to_price.back().contains(diff_seq)) continue;
      seq_to_price.back()[diff_seq] = price;
      all_diff_seqs.insert(diff_seq);
    }
    part1 += secret;
  }

  // Part 2
  i32 max_bananas = 0;
  for (const auto &diff_sequence : all_diff_seqs) {
    i32 bananas = 0;
    for (const auto &seq_price_map : seq_to_price) {
      if (!seq_price_map.contains(diff_sequence)) continue;
      bananas += seq_price_map.at(diff_sequence);
    }
    if (max_bananas < bananas) {
      max_bananas = bananas;
    }
  }
  u64 part2 = max_bananas;

  return aoc::result(part1, part2);
}