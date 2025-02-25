#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <fmt/format.h>

namespace {

using Rules = absl::btree_map<u32, absl::btree_set<u32>>;
using Pages = std::vector<u32>;
using PageSets = std::vector<Pages>;

u32 FixPages(const Pages& pages, const Rules& rules) {
  absl::flat_hash_map<u32, u32> following_pages_num;
  absl::btree_set<u32> all_pages(pages.begin(), pages.end());
  for (u32 page : pages) {
    Pages following_pages;
    absl::c_set_intersection(all_pages, rules.at(page), std::back_inserter(following_pages));
    following_pages_num[page] = following_pages.size();
  }
  Pages sorted(all_pages.begin(), all_pages.end());
  absl::c_sort(sorted, [&following_pages_num](u32 l, u32 r) {
    return following_pages_num.at(l) > following_pages_num.at(r);
  });
  return sorted.at(sorted.size()/2);
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 5>::solve() -> Result {
  // std::vector<absl::string_view> parts = absl::StrSplit(input, "\n\n", absl::SkipWhitespace());

  Rules rules;
  PageSets page_sets;
  bool rules_done = false;
  for (auto line : std::ranges::split_view(input, '\n')) {
    if (line.empty()) {
      if (!rules_done) {
        rules_done = true;
        continue;
      }
      break;
    }
  // for (auto line : absl::StrSplit(parts[0], "\n", absl::SkipWhitespace())) {
    if (!rules_done) {
      // auto result = scn::scan<u32, u32>(line, "{}|{}");
      std::vector<u32> result;
      aoc::util::FastScanList(line.data(), result);
      // CHECK(result) << "Could not parse line '"<< line <<"'.";
      rules[result[0]].insert(result[1]);
      continue;
    }
  // PageSets page_sets = aoc::util::TokenizeInput<Pages>(parts[1], [](auto line){
    Pages pages;
//    absl::c_transform(absl::StrSplit(line, ","), std::back_inserter(pages), [](auto token) {
//      auto result = scn::scan<u32>(token, "{}");
//      CHECK(result) << "Can't parse token '" <<token << "'.";
//      return result->value();
//    });
    aoc::util::FastScanList(line.data(), pages);
    page_sets.emplace_back(pages.begin(), pages.end());
  }

  // Parts 1 & 2
  u64 part1 = 0;
  u64 part2 = 0;
  for (const auto& pages : page_sets) {
    bool correct = true;
    for (u32 i = 0; i < pages.size() - 1; i++) {
      u32 page = pages.at(i);
      for (u32 j = i + 1; j < pages.size(); j++) {
        u32 following = pages.at(j);
        if (rules[following].contains(page)) {
          correct = false;
          break;
        }
      }
      if (!correct) break;
    }
    if (correct) {
      part1 += pages.at(pages.size() / 2);
    } else {
      part2 += FixPages(pages, rules);
    }
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 5>::PartOne() -> std::string { return "7198"; }

template<> auto advent<2024, 5>::PartTwo() -> std::string { return "4230"; }
