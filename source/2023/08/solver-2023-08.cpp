#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <re2/re2.h>

namespace {
RE2 NodePattern(R"((...) = \((...), (...)\))");
enum Move {
  LEFT,
  RIGHT
};
using Node = absl::string_view;
struct Instruction {
  Node left;
  Node right;
  [[nodiscard]] inline Node MoveLeftRight(Move move) const {
    return move == LEFT ? left : right;
  }
  friend std::ostream &operator<<(std::ostream &os, const Instruction &i) {
    return os << fmt::format("({}, {})", i.left, i.right);
  }
};
using Map = absl::flat_hash_map<Node, Instruction>;

u64 SolvePart1(Node start, const absl::flat_hash_set<Node> &targets, const Map &map, const std::vector<Move> &moves) {
  u64 result{0};
  Node node{start};
  while (!targets.contains(node)) {
    for (Move move : moves) {
      node = map.at(node).MoveLeftRight(move);
    }
    result += moves.size();
  }
  return result;
}

absl::flat_hash_map<u64, u64> Factorize(u64 x, const std::vector<u64> &primes) {
  absl::flat_hash_map<u64, u64> result{};
  u64 rem{x};
  int pi = 0;
  while (rem != 1 && pi < primes.size()) {
    u64 prime = primes[pi];
    while (rem % prime == 0) {
      result[prime]++;
      rem /= prime;
    }
    pi++;
  }
  CHECK(rem == 1) << "Need more primes.";
  return result;
}

}  // namespace

namespace fmt {

template<>
struct fmt::formatter<Instruction> : ostream_formatter {};

}  // fmt

template<>
auto advent2023::day08() -> result {
  std::string input = aoc::util::GetInput(year, 8, false, 3);
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  std::vector<Move> moves{};
  absl::c_transform(lines.at(0), std::back_inserter(moves), [](char c) { return c == 'L' ? LEFT : RIGHT; });
  absl::flat_hash_map<Node, Instruction> map;
  std::for_each(lines.begin() + 1, lines.end(), [&map](absl::string_view line) {
    Node key;
    Instruction inst;
    CHECK(RE2::FullMatch(line, NodePattern, &key, &inst.left, &inst.right))
            << "Unable to parse '" << line << "'.";
    map[key] = inst;
  });

  // Part 1
  u64 part1 = SolvePart1("AAA", {"ZZZ"}, map, moves);

  // Part 2
  u64 part2 = 1;
  absl::flat_hash_set<u64> solutions{part1};
  std::vector<Node> starts;
  absl::flat_hash_set<Node> targets;
  for (const auto &[key, _] : map) {
    if (key[2] == 'A' && key != "AAA") {
      starts.push_back(key);
    } else if (key[2] == 'Z') {
      targets.insert(key);
    }
  }
  for (const Node start : starts) {
    u64 solution = SolvePart1(start, targets, map, moves);
    solutions.insert(solution);
  }
  // Calculate Least Common Multiplier.
  u64 max_solution = *absl::c_max_element(solutions);
  std::vector<u64> primes = aoc::util::PrimeSieve(max_solution);
  absl::flat_hash_map<u64, u64> common_factors{};
  for (u64 solution : solutions) {
    auto factors = Factorize(solution, primes);
    for (auto [prime, power] : factors) {
      if (common_factors[prime] < power) common_factors[prime] = power;
    }
  }
  for (auto [prime, power] : common_factors) {
    for (int i = 0; i < power; i++) {
      part2 *= prime;
    }
  }

  return aoc::result(part1, part2);
}