#include <aoc.h>

#include <fmt/format.h>
#include <stack>

namespace {

using Device = std::string_view;
using Devices = absl::flat_hash_set<Device>;
using Connections = absl::flat_hash_map<Device, Devices>;
std::string_view YOU{"you"}, OUT{"out"}, SVR{"svr"}, DAC{"dac"}, FFT{"fft"};
using Cache = absl::flat_hash_map<std::pair<std::string_view, std::string_view>, u64>;

u64 CountPaths(const Connections& connections, std::string_view from, std::string_view to) {
  u64 result = 0;
  if (!connections.contains(from)) return 0;
  std::stack<std::string_view> stack;
  stack.push(from);
  while (!stack.empty()) {
    auto id = stack.top();
    stack.pop();
    if (id == to) {
      result++;
      continue;
    }
    if (id != OUT) stack.push_range(connections.at(id));
  }
  return result;
}

u64 CountPaths(
    const Connections& connections, std::string_view from, std::string_view to, Cache& cache) {
  auto cache_key = std::make_pair(from, to);
  if (cache.contains(cache_key)) return cache[cache_key];

  u64 result = 0;
  for (auto next : connections.at(from)) {
    if (next == to) {
      result++;
      continue;
    }
    if (next == OUT) continue;
    result += CountPaths(connections, next, to, cache);
  }
  cache[cache_key] = result;
  return result;
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 11>::solve() -> Result {
  Connections connections;
  for (auto line : input | std::views::split('\n')) {
    bool is_first = true;
    std::string_view from_id;
    for (auto token : line | std::views::split(' ')) {
      if (is_first) {
        from_id = std::string_view(token.begin(), token.begin() + 3);
        is_first = false;
        continue;
      }
      connections[from_id].emplace(token.begin(), token.end());
    }
  }

  // Part 1
  u64 part1 = CountPaths(connections, YOU, OUT);

  // Part 2
  Cache cache;
  u64 part2 = CountPaths(connections, SVR, FFT, cache) * CountPaths(connections, FFT, DAC, cache) *
                  CountPaths(connections, DAC, OUT, cache) +
              CountPaths(connections, SVR, DAC, cache) * CountPaths(connections, DAC, FFT, cache) *
                  CountPaths(connections, FFT, OUT, cache);

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 11>::PartOne() -> std::string { return "724"; }

template<> auto advent<2025, 11>::PartTwo() -> std::string { return "473930047491888"; }
