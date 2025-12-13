#include <aoc.h>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

using Lights = u32;
using Button = u32;
using Buttons = std::vector<Button>;
using Joltages = std::vector<u32>;
using Machine = std::tuple<Lights, Buttons, Joltages>;
using Machines = std::vector<Machine>;
using ButtonCombsByNumPressed = absl::flat_hash_map<u32, std::vector<u64>>;
using Patterns = std::vector<absl::flat_hash_map<Joltages, u64>>;

Lights ParseLights(std::string_view input, u64& index) {
  Lights lights = 0;
  u64 light_i = 0;
  char c;
  while ((c = input[index + light_i]) != ']') {
    if (c == '#') lights |= 1 << light_i;
    ++light_i;
  }
  index += light_i + 2;  // skip space
  return lights;
}

Button ParseButton(std::string_view input, u64& index) {
  Button button = 0;
  u32 light_index = 0;
  auto ptr = std::ranges::data(input) + index;
  auto str_end = input.data() + input.size();
  do {
    auto result = fast_float::from_chars(ptr, str_end, light_index);
    button |= (1 << light_index);
    if (*result.ptr == ')') break;
    ptr = const_cast<char*>(result.ptr) + 1;
  } while (true);
  index = std::ranges::distance(input.data(), ptr) + 3;  // skip space
  return button;
}

Joltages ParseJoltages(std::string_view input, u64& index) {
  Joltages joltages;
  auto ptr = std::ranges::data(input) + index;
  auto str_end = input.data() + input.size();
  u32 x;
  do {
    auto result = fast_float::from_chars(ptr, str_end, x);
    joltages.push_back(x);
    if (*result.ptr == '}') {
      index = std::ranges::distance(input.data(), result.ptr) + 1;
      break;
    }
    ptr = const_cast<char*>(result.ptr) + 1;
  } while (true);
  return joltages;
}

Patterns GetAllPatterns(
    const ButtonCombsByNumPressed& button_combinations, const Buttons& buttons, u32 num_joltages) {
  Patterns result(1 << num_joltages, {});
  u32 num_buttons = buttons.size();
  for (u32 num_pressed = 0; num_pressed <= num_buttons; num_pressed++) {
    for (const auto& buttons_pressed : button_combinations.at(num_pressed)) {
      Joltages pattern(num_joltages, 0);
      for (u32 b = 0; b < num_buttons; b++) {
        if (buttons_pressed & (1 << b))
          for (u32 j = 0; j < num_joltages; j++)
            if (buttons.at(b) & (1 << j)) pattern[j]++;
        u32 parity = 0;
        for (u32 parity_bit = 0; parity_bit < num_joltages; parity_bit++) {
          if (pattern[parity_bit] & 1) parity |= 1 << parity_bit;
        }
        if (!result[parity].contains(pattern)) result[parity][pattern] = num_pressed;
      }
    }
  }
  return result;
}

using Cache = absl::flat_hash_map<Joltages, u64>;
u64 SolveForPattern(const Patterns& patterns, const Joltages& goal, Cache& cache) {
  if (absl::c_all_of(goal, [](auto x) { return x == 0; })) return 0;
  if (cache.contains(goal)) return cache.at(goal);
  u64 result = 1'000'000;
  u32 goal_parity = 0;
  for (u32 parity_bit = 0; parity_bit < goal.size(); parity_bit++) {
    if (goal[parity_bit] & 1) goal_parity |= 1 << parity_bit;
  }
  for (const auto& [pattern, cost] : patterns[goal_parity]) {
    bool valid = true;
    Joltages new_goal(goal);
    for (u32 i = 0; i < goal.size(); i++) {
      if ((goal[i] < pattern[i])) {  //} || ((goal[i] % 2) != (pattern[i] % 2))) {
        valid = false;
        break;
      }
      new_goal[i] -= pattern[i];
    }
    if (!valid) continue;
    for (u32 i = 0; i < goal.size(); i++) new_goal[i] /= 2;
    result = std::min(result, cost + 2 * SolveForPattern(patterns, new_goal, cache));
  }
  cache[goal] = result;
  return result;
}

u64 SolveForButtonsAndPattern(
    const ButtonCombsByNumPressed& button_combinations, const Buttons& buttons,
    const Joltages& pattern) {
  Patterns patterns = GetAllPatterns(button_combinations, buttons, pattern.size());
  Cache cache;
  return SolveForPattern(patterns, pattern, cache);
}

}  // namespace

namespace fmt {}  // namespace fmt

template<>
auto advent<2025, 10>::solve() -> Result {
  Machines machines;
  Lights machine_lights;
  Buttons machine_buttons;
  Joltages machine_joltages;
  u64 i = 0;
  u32 max_buttons = 0;
  while (i < input.size()) {
    CHECK(input[i++] == '[') << "Unexpected beginning of line";
    machine_lights = ParseLights(input, i);
    machine_buttons.clear();
    while (input[i] == '(') {
      machine_buttons.push_back(ParseButton(input, ++i));
    }
    CHECK(input[i++] == '{') << "Expected joltages";
    machine_joltages.clear();
    machine_joltages = ParseJoltages(input, i);
    CHECK(i == input.size() || input[i++] == '\n') << "Expected new line at " << i;
    machines.emplace_back(machine_lights, machine_buttons, machine_joltages);
    if (max_buttons < machine_buttons.size()) max_buttons = machine_buttons.size();
  }

  // Part 1
  u64 part1 = 0;
  ButtonCombsByNumPressed button_combinations;
  button_combinations[0].push_back({});
  for (u32 button_toggles = 1; button_toggles < (1 << max_buttons); button_toggles++) {
    button_combinations[aoc::util::NumBits(button_toggles)].push_back(button_toggles);
  }
  for (const auto& [want_lights, buttons, _] : machines) {
    bool got_it = false;
    for (u32 total_toggles = 1; total_toggles <= buttons.size(); total_toggles++) {
      if (got_it) break;
      for (const auto& toggle_combinations : button_combinations.at(total_toggles)) {
        Lights actual_lights = 0;
        for (u32 b = 0; b < buttons.size(); b++) {
          if (toggle_combinations & (1 << b)) actual_lights ^= buttons.at(b);
        }
        if (actual_lights == want_lights) {
          part1 += total_toggles;
          got_it = true;
          break;
        }
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  for (const auto& [_, buttons, want_joltages] : machines) {
    part2 += SolveForButtonsAndPattern(button_combinations, buttons, want_joltages);
  }

  return aoc::result(part1, part2);
}

template<> auto advent<2025, 10>::PartOne() -> std::string { return "488"; }

template<> auto advent<2025, 10>::PartTwo() -> std::string { return "18771"; }
