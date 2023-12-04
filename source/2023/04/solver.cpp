#include <aoc.hpp>

#include <ostream>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

namespace {

using NumSet = std::set<u16>;
struct Card {
  u16 id;
  NumSet winning;
  NumSet have;
};
//using Card = std::pair<NumSet, NumSet>;

Card parseCard(absl::string_view line) {
  std::vector<absl::string_view> parts = absl::StrSplit(line, ':', absl::SkipWhitespace());
  u16 id{};
  auto ret = scn::scan(parts.at(0), "Card {}", id);
  CHECK(ret) << "Couldn't parse card id from '" << parts.at((0)) << "'";
  std::vector num_sets =
      aoc::util::TokenizeInput<NumSet>(
          parts.at(1),
          [](absl::string_view num_list) {
            std::vector<u16> nums;
            CHECK(scn::scan_list(num_list, nums)) << "Problem with '" << num_list << "'";
            return NumSet(nums.begin(), nums.end());
          },
          " | ");
  CHECK(num_sets.size() == 2) << "Expected 2 number lists, found " << num_sets.size();
  return {id, num_sets.at(0), num_sets.at(1)};
}

}  // namespace

template<>
auto advent2023::day04() -> result {
  std::string input = aoc::util::GetInput(year, 4);
  std::vector<Card> cards = aoc::util::TokenizeInput<Card>(
      input,
      [](absl::string_view line) { return parseCard(line); });

  // Part 1 & Part 2
  u64 part1 = 0;
  absl::flat_hash_map<u16, u64> copies;
  for (const auto &card : cards) {
    copies[card.id]++;
    std::vector<u16> intersection;
    absl::c_set_intersection(card.winning, card.have, std::back_inserter(intersection));
    if (intersection.empty()) continue;
    // Gain points for Part 1
    part1 += 1 << (intersection.size() - 1);
    // Gain extra copies for Part 2
    for (int copy = 1; copy <= intersection.size(); copy++) {
      u16 copy_id = card.id + copy;
      copies[copy_id] += copies[card.id];
    }
  }

  // Part 2
  u64 part2 = 0;
  for (const auto &card : cards) {
    part2 += copies.at(card.id);
  }

  return aoc::result(part1, part2);
}