#include <aoc.h>

namespace {

using Rules = absl::flat_hash_map<u32, absl::flat_hash_set<u32>>;
using Pages = std::vector<u32>;
using PageSets = std::vector<Pages>;

u32 FixPages(const Pages& pages, const Rules& rules) {
  const auto goal = pages.size() / 2;
  for (u32 page : pages) {
    u32 found_rules = 0;
    for (u32 other_page : pages) {
      if (page == other_page) continue;
      if (rules.at(page).contains(other_page)) {
        found_rules++;
      }
    }
    if (found_rules == goal) {
      return page;
    }
  }
  CHECK(false) << "Should never happen.";
  return 0;
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2024, 5>::solve() -> Result {
  Rules rules;
  PageSets page_sets;
  bool rules_done = false;
  for (auto line : input | std::views::split('\n')) {
    if (line.empty()) {
      rules_done = true;
      continue;
    }
    if (!rules_done) {
      std::vector<u32> result;
      aoc::util::FastScanList(line, result, "|");
      rules[result[0]].insert(result[1]);
      continue;
    }
    Pages pages;
    aoc::util::FastScanList(line, pages, ",");
    page_sets.emplace_back(pages.begin(), pages.end());
  }

  // Parts 1 & 2
  u64 part1 = 0;
  u64 part2 = 0;
  for (const auto& pages : page_sets) {
    bool correct = true;
    for (u32 i = 0; i < pages.size() - 1; i++) {
      u32 page = pages[i];
      for (u32 j = i + 1; j < pages.size(); j++) {
        u32 following = pages[j];
        if (rules[following].contains(page)) {
          correct = false;
          break;
        }
      }
      if (!correct) break;
    }
    if (correct) {
      part1 += pages[pages.size() / 2];
    } else {
      part2 += FixPages(pages, rules);
    }
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 5>::PartOne() -> std::string { return "7198"; }

template<> auto advent<2024, 5>::PartTwo() -> std::string { return "4230"; }
