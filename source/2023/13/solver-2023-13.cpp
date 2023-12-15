#include <aoc.hpp>

#include <Eigen/Dense>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace {

using Pattern = Eigen::MatrixXi;
Eigen::IOFormat PatterFormat(Eigen::StreamPrecision, 0, "", "\n");

i32 MirrorAtColAndDiff(const Pattern &p, i32 i_mirror) {
  u32 mirror_width = (i_mirror > p.cols() / 2) ? p.cols() - i_mirror : i_mirror;
  auto left_side = p(Eigen::all, Eigen::seq(i_mirror - mirror_width, i_mirror - 1));
  auto right_side = p(Eigen::all, Eigen::seq(i_mirror, i_mirror + mirror_width - 1));
  auto diff = left_side - right_side.rowwise().reverse();
  return diff.array().abs().sum();
}
i32 MirrorAtRowAndDiff(const Pattern &p, i32 j_mirror) {
  u32 mirror_height = (j_mirror > p.rows() / 2) ? p.rows() - j_mirror : j_mirror;
  auto top_side = p(Eigen::seq(j_mirror - mirror_height, j_mirror - 1), Eigen::all);
  auto bottom_side = p(Eigen::seq(j_mirror, j_mirror + mirror_height - 1), Eigen::all);
  auto diff = top_side - bottom_side.colwise().reverse();
  return diff.array().abs().sum();
}

}  // namespace

namespace fmt {

template<>
struct formatter<Eigen::WithFormat<Pattern>> : ostream_formatter {};

}  // namespace fmt

template<>
auto advent<2023, 13>::solve() -> Result {
  std::string input = GetInput();
  std::vector<Pattern> patterns;
  for (auto line_block : absl::StrSplit(input, "\n\n")) {
    std::vector<absl::string_view> lines = absl::StrSplit(line_block, "\n");
    patterns.emplace_back(lines.size(), lines.at(0).size());
    for (u32 i = 0; i < lines.size(); i++) {
      for (u32 j = 0; j < lines.at(i).size(); j++) {
        patterns.back()(i, j) = lines.at(i).at(j) == '.' ? 0 : 1;
      }
    }
  }

  // Part 1
  u64 part1 = 0;
  for (const auto &p : patterns) {
    bool mirrored = false;
    for (int i_mirror = 1; i_mirror < p.cols(); i_mirror++) {
      if (MirrorAtColAndDiff(p, i_mirror) == 0) {
        mirrored = true;
        part1 += i_mirror;
        break;
      }
    }
    if (mirrored) continue;
    for (int j_mirror = 1; j_mirror < p.rows(); j_mirror++) {
      if (MirrorAtRowAndDiff(p, j_mirror) == 0) {
        part1 += 100 * j_mirror;
        break;
      }
    }
  }

  // Part 2
  u64 part2 = 0;
  for (const auto &p : patterns) {
    bool mirrored = false;
    for (int i_mirror = 1; i_mirror < p.cols(); i_mirror++) {
      if (MirrorAtColAndDiff(p, i_mirror) == 1) {
        mirrored = true;
        part2 += i_mirror;
        break;
      }
    }
    if (mirrored) continue;
    for (int j_mirror = 1; j_mirror < p.rows(); j_mirror++) {
      if (MirrorAtRowAndDiff(p, j_mirror) == 1) {
        part2 += 100 * j_mirror;
        break;
      }
    }
  }

  return aoc::result(part1, part2);
}