#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>
#include "fmt/ostream.h"

namespace {

using Card = char;
const absl::btree_map<Card, u8> cardOrdering1{
    {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
    {'T', 10}, {'J', 11}, {'Q', 12}, {'K', 13},
    {'A', 14},
};
const absl::btree_map<Card, u8> cardOrdering2{
    {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
    {'T', 10}, {'J', 1}, {'Q', 11}, {'K', 12},
    {'A', 13},
};
template<int>
struct Hand {
  absl::string_view val;
  friend std::ostream &operator<<(std::ostream &os, const Hand &t) {
    return os << t.val;
  }
  [[nodiscard]] u8 CardValue(Card c) const;
  bool operator<(const Hand &right) const {
    for (int i = 0; i <= 4; i++) {
      if (val.at(i) == right.val.at(i)) continue;
      u8 val1 = CardValue(val.at(i));
      u8 val2 = CardValue(right.val.at(i));
      return val1 < val2;
    }
    return false;
  }
};
template<>
[[nodiscard]] u8 Hand<1>::CardValue(Card c) const {
  return cardOrdering1.at(c);
}
template<>
[[nodiscard]] u8 Hand<2>::CardValue(Card c) const {
  return cardOrdering2.at(c);
}
enum TypeEnum {
  FIVE_OF_A_KIND,
  FOUR_OF_A_KIND,
  FULL_HOUSE,
  THREE_OF_A_KIND,
  TWO_PAIR,
  ONE_PAIR,
  HIGH_CARD,
};
const absl::btree_map<TypeEnum, u8> typeOrdering{
    {FIVE_OF_A_KIND, 7},
    {FOUR_OF_A_KIND, 6},
    {FULL_HOUSE, 5},
    {THREE_OF_A_KIND, 4},
    {TWO_PAIR, 3},
    {ONE_PAIR, 2},
    {HIGH_CARD, 1},
};
struct Type {
  TypeEnum val;

  friend std::ostream &operator<<(std::ostream &os, const Type &t) {
    switch (t.val) {
      case FIVE_OF_A_KIND: return os << "FIVE OF A KIND";
      case FOUR_OF_A_KIND: return os << "FOUR OF A KIND";
      case FULL_HOUSE: return os << "FULL HOUSE";
      case THREE_OF_A_KIND: return os << "THREE OF A KIND";
      case TWO_PAIR: return os << "TWO PAIR";
      case ONE_PAIR: return os << "ONE PAIR";
      case HIGH_CARD: return os << "HIGH CARD";
    }
  }
  bool operator<(const Type &right) const {
    return typeOrdering.at(val) < typeOrdering.at(right.val);
  }
};
template<int Part>
struct HandType {
  Type type;
  Hand<Part> hand;
  bool operator<(const HandType &right) const {
    if (type.val == right.type.val) return hand < right.hand;
    return type < right.type;
  }
};
template<int Part>
struct HandBid {
  HandType<Part> handType;
  u64 bid;
  bool operator<(const HandBid &right) const {
    return handType < right.handType;
  }
};

TypeEnum GetType1(absl::string_view hand) {
  absl::flat_hash_set<Card> distinctCards{hand.begin(), hand.end()};

  if (distinctCards.size() == 1) return FIVE_OF_A_KIND;
  if (distinctCards.size() == 4) return ONE_PAIR;
  if (distinctCards.size() == 5) return HIGH_CARD;

  std::string sorted{hand};
  absl::c_sort(sorted);
  if (distinctCards.size() == 2) {
    if (sorted.at(0) == sorted.at(3) || sorted.at(1) == sorted.at(4)) {
      return FOUR_OF_A_KIND;
    }
    return FULL_HOUSE;
  }
  if (sorted.at(0) == sorted.at(2) || sorted.at(1) == sorted.at(3) || sorted.at(2) == sorted.at(4)) {
    return THREE_OF_A_KIND;
  }
  return TWO_PAIR;
}

TypeEnum GetType2(absl::string_view hand) {
  auto type1 = GetType1(hand);
  u32 js = absl::c_count_if(hand, [](char c) { return c == 'J'; });

  switch (js) {
    case 0: return type1;
    case 1:
      switch (type1) {
        case HIGH_CARD: return ONE_PAIR;
        case ONE_PAIR: return THREE_OF_A_KIND;
        case TWO_PAIR: return FULL_HOUSE;
        case THREE_OF_A_KIND: return FOUR_OF_A_KIND;
        default: return FIVE_OF_A_KIND;
      }
    case 2:
      switch (type1) {
        case ONE_PAIR: return THREE_OF_A_KIND;
        case TWO_PAIR: return FOUR_OF_A_KIND;
        default: return FIVE_OF_A_KIND;
      }
    case 3:if (type1 == THREE_OF_A_KIND) return FOUR_OF_A_KIND;
      return FIVE_OF_A_KIND;
    default:return FIVE_OF_A_KIND;
  }
}

}  // namespace

namespace fmt {

template<>
struct fmt::formatter<Type> : ostream_formatter {};
template<>
struct fmt::formatter<Hand < 1>> : ostream_formatter {
};
template<>
struct fmt::formatter<Hand<2>> : ostream_formatter {};

}  // namespace fmt

template<>
auto advent<2023, 7>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  std::vector<HandBid<1>> handBids = aoc::util::TokenizeInput<HandBid<1>>(input, [](absl::string_view line) {
    HandBid<1> handBid{};
    CHECK(scn::scan(line, "{} {}", handBid.handType.hand.val, handBid.bid));
    CHECK(handBid.handType.hand.val.size() == 5);
    handBid.handType.type.val = GetType1(handBid.handType.hand.val);
    return handBid;
  });

  // Part 1
  u64 part1 = 0;
  absl::btree_map<HandType<1>, u64> sortedHandBids1{};
  for (const auto &handBid : handBids) {
    sortedHandBids1[handBid.handType] = handBid.bid;
  }
  u64 rank = 1;
  for (const auto &[handType, bid] : sortedHandBids1) {
    part1 += rank++ * bid;
  }

  // Part 2
  u64 part2 = 0;
  std::vector<HandBid<2>> handBids2{};
  absl::btree_map<HandType<2>, u64> sortedHandBids2{};
  for (const auto &handBid1 : handBids) {
    HandType<2> handType2{};
    handType2.hand.val = handBid1.handType.hand.val;
    handType2.type.val = GetType2(handBid1.handType.hand.val);
    sortedHandBids2[handType2] = handBid1.bid;
  }
  rank = 1;
  for (const auto &[handType, bid] : sortedHandBids2) {
    part2 += rank++ * bid;
  }
  return aoc::result(part1, part2);
}