#include <aoc.h>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <fmt/format.h>

namespace {

using Computer = absl::string_view;
using Computers = absl::btree_set<Computer>;
using Connection = absl::btree_set<Computer>;
using Connections = absl::btree_set<Connection>;
using ConnectedWith = absl::btree_map<Computer, Computers>;

Computer ChoosePivot(const ConnectedWith &connected_with, const Computers &p, const Computers &x) {
  u32 most_connections = 0;
  Computer pivot;
  for (auto u : p) {
    u32 connections = connected_with.at(u).size();
    if (most_connections < connections) {
      most_connections = connections;
      pivot = u;
    }
  }
  for (auto u : x) {
    u32 connections = connected_with.at(u).size();
    if (most_connections < connections) {
      most_connections = connections;
      pivot = u;
    }
  }
  return pivot;
}

void BronKerbosch2(const ConnectedWith &connected_with,
                   Computers r,
                   Computers p,
                   Computers x,
                   std::vector<Computers> &solutions) {
  if (p.empty() && x.empty()) {
    solutions.emplace_back(r.begin(), r.end());
    return;
  }
  auto u = ChoosePivot(connected_with, p, x);
  Computers p_copy = p;
  for (auto v : p_copy) {
    if (connected_with.at(u).contains(v)) continue;
    Computers ri = r;
    ri.insert(v);
    std::vector<Computer> pl;
    absl::c_set_intersection(p, connected_with.at(v), std::back_inserter(pl));
    Computers pi{pl.begin(), pl.end()};
    std::vector<Computer> xl;
    absl::c_set_intersection(x, connected_with.at(v), std::back_inserter(xl));
    Computers xi{xl.begin(), xl.end()};
    BronKerbosch2(connected_with, ri, pi, xi, solutions);
    p.erase(v);
    x.insert(v);
  }
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 23>::solve() -> Result {
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  Connections connections;
  Computers computers;
  ConnectedWith connected_with;
  for (auto line : lines) {
    std::vector<Computer> comps = absl::StrSplit(line, "-", absl::SkipWhitespace());
    CHECK(comps.size() == 2) << "Unexpected number of computers in connection '" << line << "'.";
    computers.insert(comps.at(0));
    computers.insert(comps.at(1));
    connections.emplace(comps.begin(), comps.end());
    connected_with[comps.at(0)].insert(comps.at(1));
    connected_with[comps.at(1)].insert(comps.at(0));
  }

  // Part 1
  Connections interconnections;
  for (const auto &comp : computers) {
    if (comp.at(0) != 't') continue;
    std::vector<Computer> others(connected_with.at(comp).begin(), connected_with.at(comp).end());
    for (u32 i = 0; i < others.size(); i++) {
      for (u32 j = i + 1; j < others.size(); j++) {
        Connection connection{};
        connection.insert(others.at(i));
        connection.insert(others.at(j));
        if (connections.contains(connection)) {
          connection.insert(comp);
          interconnections.insert(connection);
        }
      }
    }
  }
  u64 part1 = interconnections.size();

  // Part 2
  std::vector<Computers> cliques;
  BronKerbosch2(connected_with, {}, computers, {}, cliques);
  u32 max_clique_size = 0;
  Computers clique;
  for (const auto& c : cliques) {
    if (c.size() > max_clique_size) {
      max_clique_size = c.size();
      clique = c;
    }
  }
  auto part2 = absl::StrJoin(clique, ",");

  return aoc::result(part1, part2);
}

template<> auto advent<2024, 23>::PartOne() -> std::string { return "1370"; }

template<> auto advent<2024, 23>::PartTwo() -> std::string { return "am,au,be,cm,fo,ha,hh,im,nt,os,qz,rr,so"; }
