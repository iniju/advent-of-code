#include <aoc.hpp>

#include <absl/container/btree_set.h>
#include <absl/container/btree_map.h>
#include <absl/hash/hash.h>
#include <fmt/format.h>

namespace {

using Id = absl::string_view;
using Ids = std::vector<Id>;
struct Link {
  Id a{};
  Id b{};
  Link(Id _a, Id _b) {
    if (_a < _b) {
      a = _a;
      b = _b;
    } else {
      a = _b;
      b = _a;
    }
  }
  Link(const Link &o) : a(o.a), b(o.b) {}
  [[nodiscard]] bool HasId(Id x) const { return a == x || b == x; }
  [[nodiscard]] Id Other(Id x) const {
    if (!HasId(x)) CHECK(false) << fmt::format("Asked for counterpart of {} in {}-{}.", x, a, b);
    return x == a ? b : a;
  }
  bool operator<(const Link &o) const { return a < o.a || (a == o.a && b < o.b); }
  bool operator==(const Link &o) const { return a == o.a && b == o.b; }
};
using LinkSet = absl::btree_set<Link>;
using Flows = absl::btree_map<Link, u64>;
using Graph = absl::btree_map<Id, LinkSet>;

}  // namespace

namespace fmt {

template<>
struct formatter<Link> : formatter<string_view> {
  auto format(const Link &link, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("[{} {}]", link.a, link.b), ctx);
  }
};

}  // namespace fmt

namespace {

std::tuple<u64, Flows> EdmondsKarpMaxFlow(const Graph &graph, Id source, Id sink) {
  u64 flow = 0;
  Flows flows;
  absl::btree_map<Id, Link> pred;
  do {
    std::deque<Id> q;
    q.push_back(source);
    pred.clear();
    while (!q.empty()) {
      auto cur = q.front();
      q.pop_front();
      for (const auto &link : graph.at(cur)) {
        auto t = link.Other(cur);
        if (pred.contains(t) || t == source) continue;
        if (!flows.contains(link)) {
          flows[link] = 0;
        }
        if (flows.at(link) < 1) {
          pred.emplace(t, link);
          q.push_back(t);
        }
      }
    }
    if (pred.contains(sink)) {
      u64 df = 1'000'000.0;
      auto n = sink;
      while (pred.contains(n)) {
        auto e = pred.at(n);
        df = std::min(df, 1 - flows.at(e));
        n = e.Other(n);
      }
      n = sink;
      while (pred.contains(n)) {
        auto e = pred.at(n);
        flows[e] += df;
        n = e.Other(n);
      }
      flow += df;
    }
  } while (pred.contains(sink));

  return {flow, flows};
}

u64 CountGraphSpan(const Graph &graph, Id from, const LinkSet &cut) {
  std::deque<Id> q;
  q.push_back(from);
  absl::btree_set<Id> visited;
  while (!q.empty()) {
    auto p = q.front();
    q.pop_front();
    visited.insert(p);
    for (const auto &e : graph.at(p)) {
      if (cut.contains(e)) continue;
      auto t = e.Other(p);
      if (visited.contains(t)) continue;
      q.push_back(t);
    }
  }
  return visited.size();
}

}  // namespace

template<>
auto advent<2023, 25>::solve() -> Result {
  std::string input = GetInput(false);
  Ids ids{};
  absl::btree_set<Id> idSet{};
  Graph graph{};
  Flows flows{};
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  for (auto line : lines) {
    Id a{line.substr(0, 3)};
    ids.push_back(a);
    std::vector<Id> bs = absl::StrSplit(line.substr(5), " ");
    for (auto b : bs) {
      idSet.insert(b);
      Link link{a, b};
      flows.emplace(link, 0);
      graph[a].insert(link);
      graph[b].insert(link);
    }
  }
  for (const Id &id : idSet) ids.push_back(id);

  // Part 1
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, static_cast<int>(ids.size() - 1));

  absl::btree_set<Link> min_cut;
  absl::btree_map<Link, u32> freq{};
  for (i32 i = 0; i < 100; i++) {
    auto ret = EdmondsKarpMaxFlow(graph, ids.at(distrib(gen)), ids.at(distrib(gen)));
    while (std::get<0>(ret) != 3) {
      ret = EdmondsKarpMaxFlow(graph, ids.at(distrib(gen)), ids.at(distrib(gen)));
    }
    for (const auto &[e, f] : std::get<1>(ret)) {
      if (f > 0) freq[e]++;
    }
    u32 max_frq = 0;
    min_cut.clear();
    for (const auto &[l, f] : freq) {
      if (max_frq < f) {
        max_frq = f;
        min_cut.clear();
        min_cut.insert(l);
      } else if (max_frq == f) {
        min_cut.insert(l);
      }
    }
    if (min_cut.size() == 3) {
      break;
    }
  }
  u64 part1 = CountGraphSpan(graph, min_cut.begin()->a, min_cut) * CountGraphSpan(graph, min_cut.begin()->b, min_cut);

  return aoc::result(part1, 0);
}