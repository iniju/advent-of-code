#include <aoc.h>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <fmt/format.h>

namespace {

enum Orientation : char {
  VER = 'V',
  HOR = 'H',
};

using Tile = std::pair<i64, i64>;
using Tiles = std::vector<Tile>;
using Segments = absl::btree_map<i64, i64>;
using PlacedSegments = absl::btree_map<i64, Segments>;
//using CrossingHorCorners = absl::btree_set<std::pair<i64, i64>>;
//using HorizontalCrossings = absl::btree_map<i64, absl::btree_set<i64>>;
using Perimeter = absl::flat_hash_set<std::pair<i64, i64>>;
using Perimeter2 = absl::flat_hash_set<i64>;

struct Sgmt {
  i64 x;
  i64 y;
  Orientation o;
  i64 span;
};
using Sgmts = std::vector<Sgmt>;

//void CalcScanMap(
//    const PlacedSegments& ver, const CrossingHorCorners& is_crossing, i64 from_y, i64 to_y,
//    i64 from_x, i64 to_x, HorizontalCrossings& scan) {
//  fmt::print("{} - {}\n", from_y, to_y);
//  for (i64 scan_y = from_y; scan_y <= to_y; scan_y++) {
//    if (scan_y % 1000 == 0) {
//      fmt::print("{}\n", scan_y);
//      fflush(stdout);
//    }
//    for (i64 scan_x = from_x; scan_x <= to_x; scan_x++) {
//      if (!ver.contains(scan_x)) continue;
//      const auto& vert_at_x = ver.at(scan_x);
//      auto it = vert_at_x.upper_bound(scan_y);
//      if (it == vert_at_x.begin()) continue;
//      //      if (it == vert_at_x.end()) {
//      it--;
//      if (it->first == scan_y && is_crossing.contains(std::make_pair(scan_x, scan_y))) {
//        scan[scan_y].insert(scan_x);
//      } else if (it->second == scan_y && is_crossing.contains(std::make_pair(scan_x, scan_y))) {
//        scan[scan_y].insert(scan_x);
//      } else if (it->first < scan_y && it->second > scan_y) {
//        scan[scan_y].insert(scan_x);
//      }
//    }
//  }
//}
//
//bool IsXYInside(
//    const PlacedSegments& ver, const PlacedSegments& hor, i64 x, i64 y,
//    const HorizontalCrossings& crossing_scan) {
//  // Check if part of vertical segment.
//  if (ver.contains(x)) {
//    const auto& segments = ver.at(x);
//    if (segments.contains(y)) return true;
//    auto it = segments.upper_bound(y);
//    if (it != segments.begin()) {
//      it--;
//      CHECK(it->first < y) << "bad logic";
//      if (it->second >= y) return true;
//    }
//  }
//  // Check if part of horizontal segment.
//  if (hor.contains(y)) {
//    const auto& segments = hor.at(y);
//    if (segments.contains(x)) return true;
//    auto it = segments.upper_bound(x);
//    if (it != segments.begin()) {
//      it--;
//      CHECK(it->first < x) << "bad logic";
//      if (it->second >= x) return true;
//    }
//  }
//  if (!crossing_scan.contains(y)) return false;
//  i64 vertical_crosses =
//      std::ranges::distance(crossing_scan.at(y).begin(), crossing_scan.at(y).upper_bound(x));
//  //  for (i64 scan_x = x; scan_x >= 0; scan_x--) {
//  //    if (!ver.contains(scan_x)) continue;
//  //    const auto& vert_at_x = ver.at(scan_x);
//  //    auto it = vert_at_x.lower_bound(y);
//  //    if (it == vert_at_x.end()) {
//  //      it--;
//  //    } else if (it->first == y) {
//  //      vertical_crosses++;
//  //      continue;
//  //    } else if (it == vert_at_x.begin()) {
//  //      continue;
//  //    } else {
//  //      it--;
//  //    }
//  //    if (it->second >= y) vertical_crosses++;
//  //  }
//  return vertical_crosses & 1;
//}

bool IsRectInside(const Perimeter2& perimeter, i64 width, i64 x1, i64 y1, i64 x2, i64 y2) {
  for (i64 x = x1; x <= x2; x++) {
//    if (perimeter.contains(std::make_pair(x, y1))) return false;
//    if (perimeter.contains(std::make_pair(x, y2))) return false;
    if (perimeter.contains(x + y1 * width)) return false;
    if (perimeter.contains(x + y2 * width)) return false;
  }
  for (i64 y = y1 + 1; y <= y2 - 1; y++) {
//    if (perimeter.contains(std::make_pair(x1, y))) return false;
//    if (perimeter.contains(std::make_pair(x2, y))) return false;
    if (perimeter.contains(x1 + y * width)) return false;
    if (perimeter.contains(x2 + y * width)) return false;
  }
  return true;
}

void FindPerimeter(const Sgmts& segments, i64 min_x, i64 width, Perimeter2& perimeter) {
  u32 start = 0;
  while (segments[start].x != min_x || segments[start].o == HOR) start++;
  u32 i = start;
  u32 out_x = min_x - 1;
  u32 out_y = segments[i].y;
  do {
    u32 prev = i == 0 ? segments.size() - 1 : i - 1;
    u32 next = (i == segments.size() - 1) ? 0 : i + 1;

    switch (segments[i].o) {
      case VER: {
        i32 step = aoc::signum(segments[i].span);
        for (i32 d_y = 0; d_y != segments[i].span + step; d_y += step) {
          out_y = segments[i].y + d_y;
          if (d_y == 0) {
            if ((out_x - segments[i].x) * segments[prev].span > 0) {
//              perimeter.emplace(out_x, out_y);
              perimeter.insert(out_y * width + out_x);
            }
          } else if (d_y == segments[i].span) {
            if ((out_x - segments[i].x) * segments[next].span < 0) {
//              perimeter.emplace(out_x, out_y);
              perimeter.insert(out_y * width + out_x);
              out_y += step;
//              perimeter.emplace(out_x, out_y);
              perimeter.insert(out_y * width + out_x);
            } else {
              out_y -= step;
            }
            out_x = segments[next].x;
          } else {
//            perimeter.emplace(out_x, out_y);
            perimeter.insert(out_y * width + out_x);
          }
        }
        break;
      }
      case HOR: {
        i32 step = aoc::signum(segments[i].span);
        for (i32 d_x = 0; d_x != segments[i].span + step; d_x += step) {
          out_x = segments[i].x + d_x;
          if (d_x == 0) {
            if ((out_y - segments[i].y) * segments[prev].span > 0) {
//              perimeter.emplace(out_x, out_y);
              perimeter.insert(out_y * width + out_x);
            }
          } else if (d_x == segments[i].span) {
            if ((out_y - segments[i].y) * segments[next].span < 0) {
//              perimeter.emplace(out_x, out_y);
              perimeter.insert(out_y * width + out_x);
              out_x += step;
//              perimeter.emplace(out_x, out_y);
              perimeter.insert(out_y * width + out_x);
            } else {
              out_x -= step;
            }
            out_y = segments[next].y;
          } else {
//            perimeter.emplace(out_x, out_y);
            perimeter.insert(out_y * width + out_x);
          }
        }
        break;
      }
    }

    i = next;
  } while (i != start);
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 9>::solve() -> Result {
//  GetInput(true);
  Tiles tiles;
  PlacedSegments vertical, horizontal;
  u64 min_x = -1, min_y = -1, max_x = 0, max_y = 0;
  Sgmts sgmts;
  for (auto line : input | std::views::split('\n')) {
    i64 x, y;
    auto ptr = std::ranges::data(line);
    auto line_end = ptr + std::ranges::size(line);
    auto result = fast_float::from_chars(ptr, line_end, x);
    fast_float::from_chars(result.ptr + 1, line_end, y);
    if (!tiles.empty()) {
      auto [back_x, back_y] = tiles.back();
      if (x == back_x) {
        sgmts.emplace_back(back_x, back_y, VER, y - back_y);
        if (y < back_y) vertical[x][y] = back_y;
        else
          vertical[x][back_y] = y;
      } else {
        sgmts.emplace_back(back_x, back_y, HOR, x - back_x);
        if (x < back_x) horizontal[y][x] = back_x;
        else
          horizontal[y][back_x] = x;
      }
    }
    tiles.emplace_back(x, y);
    if (min_x > x) min_x = x;
    if (max_x < x) max_x = x;
    if (min_y > y) min_y = y;
    if (max_y < y) max_y = y;
  }
  auto [front_x, front_y] = tiles.front();
  auto [back_x, back_y] = tiles.back();
  if (front_x == back_x) {
    sgmts.emplace_back(back_x, back_y, VER, front_y - back_y);
    if (front_y < back_y) vertical[front_x][front_y] = back_y;
    else
      vertical[front_x][back_y] = front_y;
  } else {
    sgmts.emplace_back(back_x, back_y, HOR, front_x - back_x);
    if (front_x < back_x) horizontal[front_y][front_x] = back_x;
    else
      horizontal[front_y][back_x] = front_x;
  }

  Perimeter2 perimeter;
  FindPerimeter(sgmts, min_x, max_x + 10, perimeter);

  // Part 1 & 2
  u64 part1 = 0, part2 = 0;
  for (u32 i = 0; i < tiles.size() - 1; i++) {
    for (u32 j = i + 1; j < tiles.size(); j++) {
      u64 area = (std::llabs(tiles[i].first - tiles[j].first) + 1) *
                 (std::llabs(tiles[i].second - tiles[j].second) + 1);
      if (part1 < area) {
        part1 = area;

      }
      if (part2 < area &&
          IsRectInside(
              perimeter, max_x + 10, std::min(tiles[i].first, tiles[j].first),
              std::min(tiles[i].second, tiles[j].second), std::max(tiles[i].first, tiles[j].first),
              std::max(tiles[i].second, tiles[j].second))) {
        part2 = area;
      }
    }
  }

  //  CalcIsCrossing(vertical, horizontal, is_crossing);

  // Part 2

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 9>::PartOne() -> std::string { return "4777967538"; }

template<> auto advent<2025, 9>::PartTwo() -> std::string { return "1439894345"; }
