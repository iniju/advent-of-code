#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <fmt/format.h>
#include <re2/re2.h>

namespace {

enum class Op : char {
  AND = '&',
  OR = '|',
  XOR = '^',
};
using GateId = absl::string_view;
using Gate = struct GateStr {
  std::pair<GateId, GateId> inputs;
  GateId output;
  Op op{Op::AND};
};
using ValMap = absl::btree_map<GateId, bool>;
using GateIdSet = absl::btree_set<GateId>;
using GateIdList = std::vector<GateId>;
using GateDefs = absl::btree_map<GateId, Gate>;

bool ApplyOp(Op op, bool a, bool b) {
  switch (op) {
    case Op::AND: return a & b;
    case Op::XOR: return a ^ b;
    case Op::OR: return a | b;
  }
}

u64 SetToNum(const GateIdList &ids, const ValMap &values) {
  u64 result = 0;
  u64 index = 1;
  for (auto id : ids) {
    if (values.contains(id) && values.at(id)) result |= index;
    index <<= 1;
  }
  return result;
}
void NumToSet(u64 value, const GateIdList &ids, ValMap &values) {
  for (auto id : ids) {
    values[id] = value & 1;
    value >>= 1;
  }
}

void CalcValues(const GateDefs &gates, const GateIdSet &ids, ValMap &values, GateIdSet &used) {
  GateIdSet todo{};
  used.clear();
  for (auto id : ids) {
    if (values.contains(id)) continue;
    todo.insert(id);
  }
  while (!todo.empty()) {
    GateIdSet done{};
    for (auto id : todo) {
      const auto &gate = gates.at(id);
      if (values.contains(gate.inputs.first) && values.contains(gate.inputs.second)) {
        values[gate.output] = ApplyOp(gate.op, values.at(gate.inputs.first), values.at(gate.inputs.second));
        used.insert(gate.output);
        done.insert(gate.output);
      }
    }
    for (auto id : done) todo.erase(id);
  }
}
void CalcGates(const GateDefs &gates, const GateIdList &gate_ids, ValMap &values, GateIdSet &used) {
  std::deque<GateId> todo{gate_ids.begin(), gate_ids.end()};
  used.clear();
  GateIdSet done{};
  while (!todo.empty()) {
    for (auto id : todo) {
      if (done.contains(id)) continue;
      const auto &gate = gates.at(id);
      if (values.contains(gate.inputs.first) && values.contains(gate.inputs.second)) {
        values[gate.output] = ApplyOp(gate.op, values.at(gate.inputs.first), values.at(gate.inputs.second));
        used.insert(gate.output);
        done.insert(gate.output);
        continue;
      }
      if (!values.contains(gate.inputs.first)) {
        todo.push_front(gate.inputs.first);
      }
      if (!values.contains(gate.inputs.second)) {
        todo.push_front(gate.inputs.second);
      }
      break;
    }
    std::deque<GateId> new_todo;
    for (auto id : todo) if (!done.contains(id)) new_todo.push_back(id);
    std::swap(todo, new_todo);
  }
}

bool IsValid(const GateDefs &gates, GateId id) {
  if (!gates.contains(id)) return true;
  const auto &gate = gates.at(id);
  if (id.at(0) == 'z' && id != "z45") {
    return gate.op == Op::XOR;
  }
  if (gate.op == Op::XOR) {
    if (gate.output.at(0) != 'z') {
      if (gate.inputs.first.at(0) != 'x' && gate.inputs.first.at(0) != 'y') return false;
      for (const auto &other : gates) {
        if (other.second.inputs.first != id && other.second.inputs.second != id) continue;
        if (other.second.op == Op::OR) return false;
      }
    }
  } else if (gate.op == Op::AND) {
    if ((gate.inputs.first.at(0) == 'x' || gate.inputs.second.at(0) == 'x') &&
        gate.inputs.first != "x00" && gate.inputs.second != "x00") {
      for (const auto &other : gates) {
        if (other.second.inputs.first != id && other.second.inputs.second != id) continue;
        if (other.second.op != Op::OR) return false;
      }
    }
  }
  return true;
}

}  // namespace

namespace fmt {

template<>
struct formatter<Op> : formatter<char> {
  auto format(Op op, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(op), ctx);
  }
};

}  // namespace fmt

template<>
auto advent<2024, 24>::solve() -> Result {
  std::vector<absl::string_view> parts = absl::StrSplit(input, "\n\n", absl::SkipWhitespace());

  ValMap values{};
  GateIdSet all_gate_ids{};
  GateIdList xs{};
  GateIdList ys{};
  for (auto line : absl::StrSplit(parts.at(0), "\n", absl::SkipWhitespace())) {
    CHECK(line.size() == 6) << "Unexpected format for line '" << line << "'.";
    auto gate_id = line.substr(0, 3);
    values[gate_id] = line.at(5) == '1';
    all_gate_ids.insert(gate_id);
    if (gate_id.at(0) == 'x') xs.push_back(gate_id);
    else if (gate_id.at(0) == 'y') ys.push_back(gate_id);
  }

  GateIdList zs{};
  GateDefs all_gates{};
  for (auto line : absl::StrSplit(parts.at(1), "\n", absl::SkipWhitespace())) {
    Gate gate;
    u32 gate_len = 3;
    switch (line.at(4)) {
      case 'A': gate.op = Op::AND;
        break;
      case 'X': gate.op = Op::XOR;
        break;
      case 'O': gate.op = Op::OR;
        gate_len = 2;
        break;
      default: CHECK(false) << "Unexpected gate format '" << line << "'.";
    }
    gate.inputs = std::make_pair(line.substr(0, 3), line.substr(5 + gate_len, 3));
    gate.output = line.substr(12 + gate_len, 3);
    all_gates[gate.output] = gate;
    all_gate_ids.insert(gate.output);
    if (gate.output.at(0) == 'z') zs.push_back(gate.output);
  }
  absl::c_sort(xs);
  absl::c_sort(ys);
  absl::c_sort(zs);

  // Part 1
  GateIdSet used{};
  CalcGates(all_gates, zs, values, used);
  u64 part1 = SetToNum(zs, values);

  // Part 2
  GateIdSet invalid{};
  for (auto id : all_gate_ids) {
    if (!IsValid(all_gates, id)) {
      invalid.insert(id);
    }
  }
//  absl::string_view part2{absl::StrJoin(invalid, ",")};

  return aoc::result(part1, absl::StrJoin(invalid, ","));
}