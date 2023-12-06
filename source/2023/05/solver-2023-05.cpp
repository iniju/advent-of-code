#include <aoc.hpp>

#include <ostream>
#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <absl/container/flat_hash_map.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace {

// Representation of MapTypes, ordered by application order.
enum MapType {
  UNKNOWN,
  SEED_SOIL,
  SOIL_FERT,
  FERT_WATR,
  WATR_LGHT,
  LGHT_TEMP,
  TEMP_HUMD,
  HUMD_LOCT,
};
absl::string_view GetMapTypeName(MapType m) {
  switch (m) {
    case MapType::SEED_SOIL: return "seed-to-soil";
    case MapType::SOIL_FERT: return "soil-to-fert";
    case MapType::FERT_WATR: return "fert-to-watr";
    case MapType::WATR_LGHT: return "watr-to-lght";
    case MapType::LGHT_TEMP: return "lght-to-temp";
    case MapType::TEMP_HUMD: return "temp-to-humd";
    case MapType::HUMD_LOCT: return "humd-to-loct";
    default: return "UNKNOWN";
  }
}
// Name to MapType mapping.
const absl::flat_hash_map<std::string, MapType> mapTypes{
    {"seed-to-soil", SEED_SOIL},
    {"soil-to-fertilizer", SOIL_FERT},
    {"fertilizer-to-water", FERT_WATR},
    {"water-to-light", WATR_LGHT},
    {"light-to-temperature", LGHT_TEMP},
    {"temperature-to-humidity", TEMP_HUMD},
    {"humidity-to-location", HUMD_LOCT},
};
// An inclusive range of numbers, e.g. [3, 7] represents the set {3, 4, 5, 6, 7}
struct Range {
  u64 start;
  u64 end;
  friend std::ostream &operator<<(std::ostream &os, const Range &r) {
    return os << "[" << r.start << ", " << r.end << "]";
  }
};

// Comparator for RangeSet. Since ranges don't overlap, we just need to compare the start number to sort them.
struct RangeLess {
  bool operator()(const Range &lhs, const Range &rhs) const {
    return lhs.start < rhs.start;
  }
};
// An ordered container of Ranges. Implicit assumption is that all ranges in the container do not overlap.
using RangeSet = absl::btree_set<Range, RangeLess>;
// An ordered pairing of Ranges to whether they were mapped or not.
using MappedRangeSet = absl::btree_map<Range, bool, RangeLess>;
// A mapping of Range-from to Range-to.
// Implicit assumption that range-from and range-to have the same number of elements.
using Mapping = absl::btree_map<Range, Range, RangeLess>;
// The holder of all mappings, keyed by MapType.
absl::flat_hash_map<MapType, Mapping> maps{};

// Parses the first line of input.
std::vector<u64> ParseSeeds(absl::string_view line) {
  CHECK(line.substr(0, 7) == "seeds: ") << "Not a seeds line ('" << line << "').";
  std::vector<u64> seeds;
  CHECK(scn::scan_list(line.substr(7), seeds)) << "Could not parse seeds.";
  return seeds;
}
// Maps a single value.
u64 MapValue(u64 value, const Range &mapFrom, const Range &mapTo) {
  if (value < mapFrom.start || value > mapFrom.end) return value;
  return value - mapFrom.start + mapTo.start;
}
// Maps a range of values.
// Depending on how the input and mapFrom ranges overlap, up to 3 ranges can be returned. These are returned as a
// MappedRangeSet, where the resulting ranges map to a bool of whether they overlapped with the mapFrom or not.
MappedRangeSet MapRange(const Range &range, const Range &mapFrom, const Range &mapTo) {
  if (range.start < mapFrom.start) {
    if (range.end < mapFrom.start) {
      return {{range, false}};
    } else if (range.end <= mapFrom.end) {
      return {
          {{range.start, mapFrom.start - 1}, false},
          {{mapTo.start, MapValue(range.end, mapFrom, mapTo)}, true}
      };
    } else {
      return {
          {{range.start, mapFrom.start - 1}, false},
          {{mapTo.start, mapTo.end}, true},
          {{mapFrom.end + 1, range.end}, false}
      };
    }
  } else if (range.start <= mapFrom.end) {
    if (range.end <= mapFrom.end) {
      return {{{MapValue(range.start, mapFrom, mapTo), MapValue(range.end, mapFrom, mapTo)}, true}};
    } else {
      return {
          {{MapValue(range.start, mapFrom, mapTo), mapTo.end}, true},
          {{mapFrom.end + 1, range.end}, false}
      };
    }
  } else {
    return {{range, false}};
  }
}
RangeSet SplitRangeSet(const Range &range, const Mapping &mapping) {
  absl::btree_set<u64> ends;
  ends.insert(range.start);
  ends.insert(range.end);
  RangeSet splitBySet{};
  for (const auto& [mapFrom, _] : mapping) {
    splitBySet.insert(mapFrom);
  }
  for (const auto& splitBy : splitBySet) {
    ends.insert(splitBy.start);
    ends.insert(splitBy.end);
  }
  RangeSet result;
  auto it = ends.lower_bound(range.start);
  while (it != ends.end() && *it < range.end) {
    result.insert({*it++, *it++});
  }
  return result;
}

u64 GetLocationForSeed(u64 seed) {
  u64 result{seed};
  for (MapType m = SEED_SOIL; m <= HUMD_LOCT; m = (MapType) ((int) m + 1)) {
    const auto &map = maps.at(m);
    for (const auto &[mapFrom, mapTo] : map) {
      auto mappedValue = MapRange({result, result}, mapFrom, mapTo);
      CHECK(mappedValue.size() == 1);
      if (mappedValue.begin()->second) {
        result = mappedValue.begin()->first.start;
        break;
      }
    }
  }
  return result;
}

RangeSet GetLocationForSeedRange(const Range &seedRange) {
  RangeSet input{};
  RangeSet result{};
  result.insert(seedRange);
  for (MapType m = SEED_SOIL; m <= HUMD_LOCT; m = (MapType) ((int) m + 1)) {
    std::swap(result, input);
    result.clear();
    for (const auto &inputRange : input) {
      RangeSet splitRanges = SplitRangeSet(inputRange, maps.at(m));
      for (const auto &split : splitRanges) {
        bool mapped = false;
        for (const auto &[mapFrom, mapTo] : maps.at(m)) {
          if (mapFrom.start <= split.start && mapFrom.end >= split.end) {
            u64 shift = mapTo.start - mapFrom.start;
            result.insert({split.start + shift, split.end + shift});
            mapped = true;
            break;
          }
        }
        if (!mapped) result.insert(split);
      }
    }
  }
  return result;
}

}  // namespace

namespace fmt {

template<>
struct fmt::formatter<Range> : ostream_formatter {};

}

template<>
auto advent2023::day05() -> result {
  std::string input = aoc::util::GetInput(year, 5, false);
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  std::vector<u64> seeds = ParseSeeds(lines.at(0));
  MapType mapType{UNKNOWN};
  for (int l = 1; l < lines.size(); l++) {
    auto line = lines.at(l);
    if (line.at(0) < '0' || line.at(0) > '9') {
      auto ret = scn::scan_value<std::string>(line);
      CHECK(ret && ret.has_value()) << "Could not parse map type '" << line << "'";
      mapType = mapTypes.at(ret.value());
      continue;
    }
    u64 dst{};
    u64 src{};
    u64 len{};
    CHECK(scn::scan(line, "{}{}{}", dst, src, len)) << "Couldn't parse mapping '" << line << "'";
    maps[mapType][{src, src + len - 1}] = {dst, dst + len - 1};
  }

  // Part 1
  u64 part1 = std::numeric_limits<u64>::max();
  for (u64 seed : seeds) {
    u64 loc = GetLocationForSeed(seed);
    if (part1 > loc) {
      part1 = loc;
    }
  }

  // Part 2
  u64 part2 = std::numeric_limits<u64>::max();
  CHECK(seeds.size() % 2 == 0);
  std::vector<Range> seeds2{};
  auto it = seeds.begin();
  while (it != seeds.end()) {
    u64 start = *it++;
    u64 end = start + *it++ - 1;
    seeds2.push_back({start, end});
  }

  for (const Range &seedRange : seeds2) {
    auto locRanges = GetLocationForSeedRange(seedRange);
    if (part2 > locRanges.begin()->start) {
      part2 = locRanges.begin()->start;
    }
  }

  return aoc::result(part1, part2);
}