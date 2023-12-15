#include <aoc.hpp>

#include <absl/container/flat_hash_map.h>
#include <fmt/format.h>

namespace {

struct Lens {
  absl::string_view label;
  int focal_length;
};
using Box = std::vector<Lens>;
using Boxes = std::vector<Box>;

u32 CalculateHash(absl::string_view code) {
  u32 hash{0};
  for (char c : code) {
    hash += c;
    hash *= 17;
    hash %= 256;
  }
  return hash;
}

void AddLens(Boxes &boxes, const Lens &lens) {
  Box &box = boxes[CalculateHash(lens.label)];
  auto index = std::find_if(box.begin(), box.end(), [&](const auto &l) { return l.label == lens.label; });
  if (index != box.end()) {
    index->focal_length = lens.focal_length;
    return;
  }
  box.push_back(lens);
}

void RemoveLens(Boxes &boxes, const Lens &lens) {
  Box &box = boxes[CalculateHash(lens.label)];
  auto index = std::find_if(box.begin(), box.end(), [&](const auto &l) { return l.label == lens.label; });
  if (index == box.end()) {
    return;
  }
  box.erase(index);
}

}  // namespace

namespace fmt {

template<>
struct fmt::formatter<Lens> : formatter<string_view> {
  static auto format(const Lens &l, format_context &ctx) {
    return fmt::format_to(ctx.out(), "[{} {}]", l.label, l.focal_length);
  }
};
template<>
struct fmt::formatter<Box> : formatter<string_view> {
  static auto format(const Box &b, format_context &ctx) {
    return fmt::format_to(ctx.out(), "{}", fmt::join(b, " "));
  }
};

}  // namespace fmt

template<>
auto advent<2023, 15>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> codes = absl::StrSplit(input, ",", absl::SkipWhitespace());

  // Part 1
  u64 part1{0};
  absl::c_for_each(codes, [&part1](auto code) { part1 += CalculateHash(code); });

  // Part 2
  u64 part2{0};
  std::vector<Box> boxes{256};
  for (auto code : codes) {
    if (code.back() == '-') {
      RemoveLens(boxes, {code.substr(0, code.size() - 1), 0});
    } else {
      AddLens(boxes, {code.substr(0, code.size() - 2), code.back() - '0'});
    }
  }
  for (u32 i = 0; i < boxes.size(); i++) {
    const auto box = boxes.at(i);
    for (u32 b = 0; b < box.size(); b++) {
      part2 += (i + 1) * (b + 1) * box.at(b).focal_length;
    }
  }

  return aoc::result(part1, part2);
}