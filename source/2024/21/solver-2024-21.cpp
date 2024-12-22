#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

using KeyPad = absl::flat_hash_map<char, aoc::Pos>;
using Route = std::string;
using Routes = std::vector<Route>;
using RouteMap = absl::flat_hash_map<std::pair<char, char>, Routes>;
using BestRouteMap = absl::flat_hash_map<std::pair<char, char>, Route>;
bool IsSafeFromBlank(const aoc::Pos &from, const aoc::Pos &blank, const std::deque<aoc::Dir> &route_dir) {
  aoc::Pos p = from;
  for (auto d : route_dir) {
    p = p + aoc::MoveDir(d);
    if (p == blank) return false;
  }
  return true;
}
Routes ShortestRoutes(const KeyPad &key_pad, char from, char to) {
  if (from == to) return {"A"};
  Routes result;
  const auto &start = key_pad.at(from);
  const auto &end = key_pad.at(to);
  const auto &blank = key_pad.at(' ');
  aoc::Dir v_dir = end.i > start.i ? aoc::Dir::S : aoc::Dir::N;
  aoc::Dir h_dir = end.j > start.j ? aoc::Dir::E : aoc::Dir::W;
  u32 v_num = abs(end.i - start.i);
  u32 h_num = abs(end.j - start.j);
  std::deque<aoc::Dir> route_vh_dir, route_hv_dir;
  for (u32 v = 0; v < v_num; v++) {
    route_vh_dir.push_back(v_dir);
    route_hv_dir.push_back(v_dir);
  }
  for (u32 h = 0; h < h_num; h++) {
    route_vh_dir.push_back(h_dir);
    route_hv_dir.push_front(h_dir);
  }
  if (IsSafeFromBlank(start, blank, route_vh_dir)) {
    Route route;
    absl::c_transform(route_vh_dir, std::back_inserter(route), [](auto d) { return aoc::ToUnderlying(d); });
    route.push_back('A');
    result.push_back(route);
  }
  if (route_vh_dir != route_hv_dir && IsSafeFromBlank(start, blank, route_hv_dir)) {
    Route route;
    absl::c_transform(route_hv_dir, std::back_inserter(route), [](auto d) { return aoc::ToUnderlying(d); });
    route.push_back('A');
    result.push_back(route);
  }
  return result;
}

RouteMap GetAllValidRoutes(const KeyPad &key_pad, const std::vector<char> &keys) {
  RouteMap route_map{};
  for (char from : keys) {
    for (char to : keys) {
      auto from_to = std::make_pair(from, to);
      auto routes = ShortestRoutes(key_pad, from, to);
      for (const auto &route : routes) route_map[from_to].emplace_back(route);
    }
  }
  return route_map;
}
void DeleteLongSequences(std::vector<std::string> &routes) {
  u64 min_len = std::numeric_limits<u64>::max();
  for (const auto &route : routes) {
    if (min_len > route.size()) {
      min_len = route.size();
    }
  }
  std::erase_if(routes, [min_len](const auto &route) { return route.size() > min_len; });
}
std::vector<std::string> GetKeySequences(absl::string_view code, const RouteMap &route_map) {
  char from = 'A';
  std::vector<std::string> result{""};
  for (char to : code) {
    const auto &routes = route_map.at(std::make_pair(from, to));
    std::vector<std::string> new_result{};
    for (const auto &route : routes) {
      for (const auto &res : result) {
        new_result.push_back(res + route);
      }
    }
    std::swap(result, new_result);
    from = to;
  }
  return result;
}

std::string GetBestKeySequence(absl::string_view code, const BestRouteMap &route_map) {
  char from = 'A';
  std::string result{};
  u64 c = 0;
  while (c < code.size()) {
    char to = code.at(c);
    result += route_map.at(std::make_pair(from, to));
    from = to;
    c++;
  }
  return result;
}

using LenCacheKey = std::pair<std::pair<char, char>, u32>;
using LenCache = absl::flat_hash_map<LenCacheKey, u64>;
u64 DFSForCode(absl::string_view code, const BestRouteMap &route_map, u32 depth, LenCache& cache);
u64 DFSForPair(char from, char to, const BestRouteMap &route_map, u32 depth, LenCache& cache) {
  if (depth == 0) return 1;
  auto from_to = std::make_pair(from, to);
  LenCacheKey cache_key = std::make_pair(from_to, depth);
  if (cache.contains(cache_key)) return cache.at(cache_key);
  auto result = DFSForCode(route_map.at(from_to), route_map, depth - 1, cache);
  cache[cache_key] = result;
  return result;
}
u64 DFSForCode(absl::string_view code, const BestRouteMap &route_map, u32 depth, LenCache& cache) {
  u64 result = 0;
  char from = 'A';
  for (char to : code) {
    result += DFSForPair(from, to, route_map, depth, cache);
    from = to;
  }
  return result;
}

using CompareCache = absl::flat_hash_map<std::pair<Route, Route>, bool>;
bool Compare(const Route &a, const Route &b, const RouteMap &dir_route_map, CompareCache &cache) {
  auto cache_key = std::make_pair(a, b);
  if (cache.contains(cache_key)) return cache.at(cache_key);
  Routes a_routes{a}, b_routes{b};
  std::vector<std::string> new_routes{};
  while (a_routes.at(0).size() == b_routes.at(0).size()) {
    new_routes.clear();
    for (const auto &a_route : a_routes) {
      for (const auto &new_route : GetKeySequences(a_route, dir_route_map)) new_routes.push_back(new_route);
    }
    DeleteLongSequences(new_routes);
    std::swap(a_routes, new_routes);
    new_routes.clear();
    for (const auto &b_route : b_routes) {
      for (const auto &new_route : GetKeySequences(b_route, dir_route_map)) new_routes.push_back(new_route);
    }
    DeleteLongSequences(new_routes);
    std::swap(b_routes, new_routes);
  }
  bool result = a_routes.at(0).size() < b_routes.at(0).size();
  cache[cache_key] = result;
  cache[std::make_pair(b, a)] = !result;
  return result;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 21>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> codes = absl::StrSplit(input, "\n", absl::SkipWhitespace());

  KeyPad num_key_pad{
      {'7', {0, 0}},
      {'8', {0, 1}},
      {'9', {0, 2}},
      {'4', {1, 0}},
      {'5', {1, 1}},
      {'6', {1, 2}},
      {'1', {2, 0}},
      {'2', {2, 1}},
      {'3', {2, 2}},
      {' ', {3, 0}},
      {'0', {3, 1}},
      {'A', {3, 2}},
  };
  KeyPad dir_key_pad{
      {' ', {0, 0}},
      {'^', {0, 1}},
      {'A', {0, 2}},
      {'<', {1, 0}},
      {'v', {1, 1}},
      {'>', {1, 2}},
  };

  std::vector<char> num_keys = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A'};
  std::vector<char> dir_keys = {'^', 'v', '<', '>', 'A'};
  RouteMap num_route_map = GetAllValidRoutes(num_key_pad, num_keys);
  RouteMap dir_route_map = GetAllValidRoutes(dir_key_pad, dir_keys);
  BestRouteMap best_num_route_map, best_dir_route_map;
  CompareCache compare_cache{};
  for (auto &it : num_route_map) {
    CHECK(it.second.size() <= 2) << "Too many routes for [" << it.first.first << "," << it.first.second << "].";
    if (it.second.size() < 2) {
      best_num_route_map[it.first] = it.second.at(0);
      continue;
    } else if (Compare(it.second.at(0), it.second.at(1), dir_route_map, compare_cache)) {
      best_num_route_map[it.first] = it.second.at(0);
      std::erase(num_route_map[it.first], it.second.at(1));
    } else {
      best_num_route_map[it.first] = it.second.at(1);
      std::erase(num_route_map[it.first], it.second.at(0));
    }
  }
  for (const auto &it : dir_route_map) {
    CHECK(it.second.size() <= 2) << "Too many routes for [" << it.first.first << "," << it.first.second << "].";
    if (it.second.size() < 2) {
      best_dir_route_map[it.first] = it.second.at(0);
      continue;
    } else if (Compare(it.second.at(0), it.second.at(1), dir_route_map, compare_cache)) {
      best_dir_route_map[it.first] = it.second.at(0);
      std::erase(dir_route_map[it.first], it.second.at(1));
    } else {
      best_dir_route_map[it.first] = it.second.at(1);
      std::erase(dir_route_map[it.first], it.second.at(0));
    }
  }

//  SequenceCache cache{};
//  absl::flat_hash_map<char, u64> another_cache;
//  for (char ch : {'^', 'v', '<', '>', 'A'}) {
//    std::string code{ch};
//    for (u32 d = 0; d < 1; d++) {
//      code = GetBestKeySequence(code, best_dir_route_map);
//    }
//    another_cache[ch] = code.size();
//  }

  // Part 1 & Part 2
  u64 part1 = 0, part2 = 0;
  LenCache len_cache{};
  for (auto code : codes) {
    u64 code_value = 0;
    for (char ch : code) {
      if (ch == 'A') break;
      code_value = 10 * code_value + ch - '0';
    }
    std::string sequence = GetBestKeySequence(code, best_num_route_map);
    part1 += code_value * DFSForCode(sequence, best_dir_route_map, 2, len_cache);
    part2 += code_value * DFSForCode(sequence, best_dir_route_map, 25, len_cache);
  }

  return aoc::result(part1, part2);
}