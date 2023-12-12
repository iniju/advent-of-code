#include <aoc.hpp>

#include <absl/container/flat_hash_map.h>
#include <fmt/format.h>
#include <re2/re2.h>

namespace {

RE2 kLinePattern(R"(([.?#]+) ([0-9,]+))");
using Group = std::tuple<i32, i32>;
using Groups = std::vector<i32>;

// Checks if pattern starts with `group` number of '#' (or '?').
// If yes and there's more pattern after that, also checks that a '.' (or '?') follows that (otherwise the group would
// be greater than `group`).
// Returns the number of characters matched:
// - Match with pattern.size > group -> group + 1
// - Match with pattern.size == group -> group
// - No match -> 0
i32 MatchGroupAtStart(absl::string_view pattern, i32 group) {
  // If the pattern is smaller that the group, there's no possibility of match.
  if (pattern.size() < group) return 0;
  // If any '.' appears in pattern[0:group], there's no possibility of match.
  if (!absl::c_all_of(pattern.substr(0, group), [](char c) { return c != '.'; })) return 0;
  bool has_more = pattern.size() > group;
  // If there's more pattern than group size and a '#' is next, again no match.
  if (has_more && pattern[group] == '#') return 0;
  // Return the matched size (including the separating '.' (or '?') if it exists).
  return has_more ? group + 1 : group;
}
using Cache = absl::flat_hash_map<std::tuple<absl::string_view, Groups>, u64>;
// Count how many arrangements of the given `groups` fit in the given pattern.
u64 CountMatches(absl::string_view pattern, const Groups &groups, Cache &cache) {
  // Key for the cache, matches exactly the inputs (except the cache input).
  auto cache_key = std::make_tuple(pattern, groups);
  auto it = cache.find(cache_key);
  if (it != cache.end()) {
    // We've done this exact calculation before, return the cached result.
    return it->second;
  }
  // If no groups are passed in and pattern has no '#', count 1 arrangement (any '?' should be '.').
  if (groups.empty()) {
    if (pattern.find('#') == absl::string_view::npos) {
      return 1;
    } else {
      return 0;
    }
  }
  // If we passed no pattern, count no arrangement.
  if (pattern.empty()) return 0;

  // The actual counting logic.
  u64 matches = 0;
  // Get the first group.
  i32 group = groups[0];
  // Early exit if the pattern is too small for this group.
  if (pattern.size() < group) return 0;

  // Find the earliest and latest possible indices of the group in the pattern.
  // The group must fit in the pattern, so it has to start at latest index pattern_size - group.
  u64 latest_possible = pattern.size() - group;
  // The group must use the next available '#', so the latest possible starting point has to be the next '#'.
  u64 latest_obligatory = pattern.substr(0, latest_possible).find('#');
  if (latest_obligatory != absl::string_view::npos) {
    latest_possible = latest_obligatory;
  }
  // Iterate through the possible starting points of the group.
  for (i32 i = 0; i <= latest_possible; i++) {
    i32 matched_size = MatchGroupAtStart(pattern.substr(i), group);
    if (matched_size == 0) continue;
    // If we have a match for this group, recurse using the remaining pattern and the remaining groups.
    matches += CountMatches(pattern.substr(i + matched_size), Groups{groups.begin() + 1, groups.end()}, cache);
  }
  // Store the count matches in the cache.
  cache[cache_key] = matches;
  return matches;
}

}  // namespace

template<>
auto advent<2023, 12>::solve() -> Result {
  std::string input = GetInput(false);
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());

  std::vector<std::string> patterns1(lines.size());
  std::vector<Groups> group_sets1(lines.size());
  for (i32 i = 0; i < lines.size(); i++) {
    std::string group_str;
    CHECK(RE2::FullMatch(lines.at(i), kLinePattern, &patterns1[i], &group_str))
            << "Couldn't parse '" << lines.at(i) << "'.";
    CHECK(scn::scan_list_ex(group_str, group_sets1[i], scn::list_separator(',')))
            << "Couldn't parse '" << group_str << "'.";
  }

  // Part 1
  u64 part1 = 0;
  Cache cache{};
  for (i32 i = 0; i < patterns1.size(); i++) {
    part1 += CountMatches(patterns1.at(i), group_sets1.at(i), cache);
  }

  // Part 2
  u64 part2 = 0;
  std::vector<std::string> patterns2(lines.size());
  std::vector<Groups> group_sets2(lines.size());
  for (i32 i = 0; i < patterns1.size(); i++) {
    patterns2[i] = fmt::format("{0}?{0}?{0}?{0}?{0}", patterns1.at(i));
    for (i32 j = 0; j < 5; j++) {
      for (auto g : group_sets1.at(i)) {
        group_sets2[i].push_back(g);
      }
    }
  }

  cache.clear();
  for (i32 i = 0; i < patterns2.size(); i++) {
    part2 += CountMatches(patterns2.at(i), group_sets2.at(i), cache);
  }

  return aoc::result(part1, part2);
}