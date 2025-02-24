#include <aoc.hpp>

#include <absl/container/flat_hash_set.h>
#include <fmt/format.h>

namespace {
constexpr absl::string_view kBroadcaster{"broadcaster"};
constexpr absl::string_view kRx{"rx"};
enum class ModuleType : char {
  BROADCAST = '*',
  FF = '%',
  CNJ = '&',
  UNTYPED = '-'
};
struct Module {
  absl::string_view name;
  ModuleType type;
  bool mem{false};
  std::vector<absl::string_view> targets{};
  absl::flat_hash_map<absl::string_view, bool> inputs{};
  Module() : name() {}
  explicit Module(absl::string_view _name, ModuleType _type) : name(_name), type(_type) {}
  Module(const Module &o) : name(o.name), type(o.type), mem(o.mem), targets{o.targets}, inputs(o.inputs) {}
  Module(Module &&) = default;
  bool operator==(const Module &o) const {
    return name == o.name && type == o.type && mem == o.mem && targets == o.targets && inputs == o.inputs;
  }

};
struct Pulse {
  absl::string_view from;
  bool val;
  absl::string_view to;
};
using Modules = absl::flat_hash_map<absl::string_view, Module>;
using Pulses = std::deque<Pulse>;

}  // namespace

namespace fmt {

template<>
struct formatter<ModuleType> : formatter<char> {
  auto format(ModuleType type, format_context &ctx) const {
    return formatter<char>::format(aoc::ToUnderlying(type), ctx);
  }
};
template<>
struct formatter<Module> : formatter<string_view> {
  auto format(const Module &mod, format_context &ctx) const {
    absl::string_view state;
    switch (mod.type) {
      case ModuleType::FF:
        return formatter<string_view>::format(fmt::format("{}{} [{}] ", mod.type, mod.name, mod.mem),
                                              ctx);
      case ModuleType::CNJ:
        return formatter<string_view>::format(fmt::format("{}{} {} ",
                                                          mod.type,
                                                          mod.name,
                                                          mod.inputs), ctx);
      default:return formatter<string_view>::format(fmt::format("{}{} ", mod.type, mod.name), ctx);
    }
  }
};
template<>
struct formatter<Pulse> : formatter<string_view> {
  auto format(const Pulse &pulse, format_context &ctx) const {
    return formatter<string_view>::format(fmt::format("{} -{}-> {}", pulse.from, pulse.val ? "high" : "low", pulse.to),
                                          ctx);
  }
};

}  // namespace fmt

namespace {

void TransmitPulses(const Module &mod, Pulses &pulses, bool pulse) {
  for (auto target : mod.targets) {
    Pulse p{mod.name, pulse, target};
    pulses.push_back(p);
  }
}

void AcceptPulse(Modules &mods, Pulses &pulses, bool pulse, absl::string_view from, absl::string_view target) {
  auto &mod = mods[target];
  bool all_high;
  switch (mod.type) {
    case ModuleType::FF:
      if (!pulse) {
        mod.mem = !mod.mem;
        TransmitPulses(mod, pulses, mod.mem);
      }
      break;
    case ModuleType::CNJ:CHECK(mod.inputs.contains(from))
              << "Unexpected input for " << mod.name << " from " << from << ".";
      mod.inputs[from] = pulse;
      all_high = absl::c_all_of(mod.inputs, [](const auto &kv) { return kv.second; });
      TransmitPulses(mod, pulses, !all_high);
      break;
    case ModuleType::UNTYPED:mod.mem = pulse;
      break;
    default: CHECK(false) << "Unexpected target module type for pulse '" << aoc::ToUnderlying(mod.type) << "'.";
  }
}

std::tuple<u64, u64> PressButton(Modules &mods, absl::flat_hash_map<absl::string_view, bool> &monitor) {
  u64 lows{1};
  u64 highs{0};
  std::deque<Pulse> pulses;
  for (auto target : mods.at(kBroadcaster).targets) {
    pulses.emplace_back(kBroadcaster, false, target);
  }
  while (!pulses.empty()) {
    auto pulse = pulses.front();
    pulses.pop_front();
    if (pulse.val) highs++; else lows++;
    if (monitor.contains(pulse.from) && !pulse.val) {
      monitor[pulse.from] = true;
    }
    AcceptPulse(mods, pulses, pulse.val, pulse.from, pulse.to);
  }
  return {lows, highs};
}

}  // namespace

template<>
auto advent<2023, 20>::solve() -> Result {
  std::string input = GetInput(false, 2);
  Modules modules;
  for (auto line : absl::StrSplit(input, "\n", absl::SkipWhitespace())) {
    Module mod;
    auto index = line.find(" -> ");
    mod.targets = absl::StrSplit(line.substr(index + 4), ", ");
    if (line.starts_with(kBroadcaster)) {
      mod.type = ModuleType::BROADCAST;
      mod.name = kBroadcaster;
    } else {
      switch (line.at(0)) {
        case '%': mod.type = ModuleType::FF;
          break;
        case '&': mod.type = ModuleType::CNJ;
          break;
        default: CHECK(false) << "Unexpected module type '" << line << "'.";
      }
      mod.name = line.substr(1, index - 1);
    }
    modules.emplace(mod.name, mod);
  }

  // Initialize inputs for conjunctions.
  absl::flat_hash_map<absl::string_view, absl::flat_hash_set<absl::string_view>> inputs;
  absl::flat_hash_set<absl::string_view> untyped;
  inputs[kBroadcaster] = {};
  for (const auto &mod : modules) {
    for (auto target : mod.second.targets) {
      if (!modules.contains(target)) {
        untyped.insert(target);
      }
      inputs[target].insert(mod.second.name);
    }
  }
  for (auto name : untyped) {
    Module mod{name, ModuleType::UNTYPED};
    modules.emplace(name, mod);
  }
  for (auto [to, froms] : inputs) {
    if (modules.at(to).type != ModuleType::CNJ) continue;
    for (auto from : froms) {
      modules[to].inputs.emplace(from, false);
    }
  }

  // Part 1
  u64 lows{0};
  u64 highs{0};
  absl::flat_hash_map<absl::string_view, bool> monitor{};
  for (int i = 0; i < 1000; i++) {
    auto [l, h] = PressButton(modules, monitor);
    lows += l;
    highs += h;
  }
  u64 part1 = lows * highs;

  // Part 2
  u64 part2{1};
  monitor.emplace("nl", false);
  monitor.emplace("cr", false);
  monitor.emplace("jx", false);
  monitor.emplace("vj", false);
  absl::flat_hash_map<absl::string_view, u64> required{};
  u64 press{1000};
  while (absl::c_any_of(monitor, [](const auto &kv) { return !kv.second; })) {
    press++;
    PressButton(modules, monitor);
    for (auto [mod, triggered] : monitor) {
      if (triggered) {
        required.emplace(mod, press);
      }
    }
    for (auto [mod, _] : required) monitor.erase(mod);
    if (required.size() == 4) break;
  }
  absl::flat_hash_set<u64> cycles{};
  for (auto [k, v] : required) cycles.insert(v);
  part2 = aoc::util::LCM(cycles);

  return aoc::result(part1, part2);
}