#include <aoc.hpp>

#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>
#include <re2/re2.h>

namespace {

RE2 kWorkflowPattern(R"(([^{]+){(.*)})");
RE2 kClausePattern(R"((.)([<>])(\d+):(\w+))");
RE2 kPartPattern(R"({x=(\d+),m=(\d+),a=(\d+),s=(\d+)})");

struct Clause {
  bool is_unconditional;
  char attr;
  bool is_less;
  u64 val;
  absl::string_view target;
};
struct Workflow {
  absl::string_view name;
  std::vector<Clause> clauses;
};
using Part = absl::flat_hash_map<char, u64>;
using RangedPart = absl::flat_hash_map<char, std::pair<u64, u64>>;
constexpr absl::string_view kIn{"in"};
constexpr absl::string_view kAccept{"A"};
constexpr absl::string_view kReject{"R"};

bool EvalClause(const Clause &clause, const Part &part) {
  return clause.is_less ? (part.at(clause.attr) < clause.val) : (part.at(clause.attr) > clause.val);
}

absl::string_view RunWorkflow(const Workflow &workflow, const Part &part) {
  for (const Clause &clause : workflow.clauses) {
    if (clause.is_unconditional) return clause.target;
    if (EvalClause(clause, part)) return clause.target;
  }
  CHECK(false) << "Should have matched a clause already.";
}

std::tuple<bool, bool, RangedPart, RangedPart> SplitRangedPart(const RangedPart &rp, const Clause &clause) {
  if (clause.is_unconditional) return std::make_tuple(true, false, rp, rp);
  RangedPart success{rp};
  RangedPart failure{rp};
  char attr = clause.attr;
  if (clause.is_less) {
    if (rp.at(attr).first >= clause.val) {
      return std::make_tuple(false, true, rp, rp);
    }
    if (rp.at(attr).second < clause.val) {
      return std::make_tuple(true, false, rp, rp);
    }
    success[attr].second = clause.val - 1;
    failure[attr].first = clause.val;
    return std::make_tuple(true, true, success, failure);
  }
  if (rp.at(attr).second <= clause.val) {
    return std::make_tuple(false, true, rp, rp);
  }
  if (rp.at(attr).first > clause.val) {
    return std::make_tuple(true, false, rp, rp);
  }
  success[attr].first = clause.val + 1;
  failure[attr].second = clause.val;
  return std::make_tuple(true, true, success, failure);
}

}  // namespace

namespace fmt {

template<>
struct formatter<Clause> : formatter<string_view> {
  auto format(const Clause &c, format_context &ctx) const {
    if (c.is_unconditional) return formatter<string_view>::format(c.target, ctx);
    return formatter<string_view>::format(fmt::format("{}{}{}:{}", c.attr, c.is_less ? '<' : '>', c.val, c.target),
                                          ctx);
  }
};
template<>
struct formatter<Workflow> : formatter<string_view> {
  auto format(const Workflow &w, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("{}{{{}}}", w.name, fmt::join(w.clauses, ",")), ctx);
  }
};

}  // namespace fmt

template<>
auto advent<2023, 19>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> input_parts = absl::StrSplit(input, "\n\n");
  // Parse workflows.
  absl::flat_hash_map<absl::string_view, Workflow> workflows;
  for (auto line : absl::StrSplit(input_parts.at(0), "\n")) {
    Workflow wf;
    absl::string_view clauses_str;
    CHECK(RE2::FullMatch(line, kWorkflowPattern, &wf.name, &clauses_str)) << "Unable to parse '" << line << "'.";
    wf.clauses = aoc::util::TokenizeInput<Clause>(clauses_str, [](absl::string_view clause_str) {
      Clause cl{false, '?', false, 0, ""};
      if (clause_str.find(':') == absl::string_view::npos) {
        cl.target = clause_str;
        cl.is_unconditional = true;
        return cl;
      }
      char op{'?'};
      CHECK(RE2::FullMatch(clause_str, kClausePattern, &cl.attr, &op, &cl.val, &cl.target))
              << "Unable to parse clause '" << clause_str << "'.";
      cl.is_less = op == '<';
      return cl;
    }, ",");
    workflows[wf.name] = wf;
  }
  // Parse parts.
  std::vector<Part> parts;
  for (auto line : absl::StrSplit(input_parts.at(1), "\n")) {
    Part p{};
    u64 x;
    u64 m;
    u64 a;
    u64 s;
    CHECK(RE2::FullMatch(line, kPartPattern, &x, &m, &a, &s)) << "Unable to parse part '" << line << "'.";
    p['x'] = x;
    p['m'] = m;
    p['a'] = a;
    p['s'] = s;
    parts.push_back(p);
  }

  // Part 1
  u64 part1 = 0;
  absl::string_view wf;
  for (const auto &part : parts) {
    wf = kIn;
    while (wf != kAccept && wf != kReject) {
      wf = RunWorkflow(workflows.at(wf), part);
    }
    if (wf == kAccept) {
      part1 += part.at('x') + part.at('m') + part.at('a') + part.at('s');
    }
  }

  // Part 2
  u64 part2 = 0;
  std::deque<std::tuple<RangedPart, absl::string_view>> rps;
  RangedPart init_rp{{'x', {1, 4000}}, {'m', {1, 4000}}, {'a', {1, 4000}}, {'s', {1, 4000}}};
  rps.emplace_back(init_rp, kIn);
  while (!rps.empty()) {
    auto [rp, wf_name] = rps.front();
    rps.pop_front();

    for (const auto &clause : workflows.at(wf_name).clauses) {
      auto [has_success, has_failure, success, failure] = SplitRangedPart(rp, clause);
      if (has_success) {
        if (clause.target == kAccept) {
          u64 combs = (success.at('x').second - success.at('x').first + 1)
              * (success.at('m').second - success.at('m').first + 1)
              * (success.at('a').second - success.at('a').first + 1)
              * (success.at('s').second - success.at('s').first + 1);
          part2 += combs;
        } else if (clause.target != kReject) {
          rps.emplace_back(success, clause.target);
        }
      }
      if (!has_failure) break;
      rp = failure;
    }
  }

  return aoc::result(part1, part2);
}