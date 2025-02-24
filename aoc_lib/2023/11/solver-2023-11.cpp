#include <aoc.hpp>

#include <absl/container/btree_set.h>
#include <Eigen/Dense>
#include <fmt/format.h>

namespace {

using Sky = Eigen::Matrix<u8, Eigen::Dynamic, Eigen::Dynamic>;
using Galaxy = std::pair<i32, i32>;
using IndexSet = absl::btree_set<i32>;
//Eigen::IOFormat SkyFormat(Eigen::StreamPrecision, 0, "", "\n");
struct GalaxyFinder {
  std::vector<Galaxy> finds{};
  // called for the first coefficient
  void init(i32 value, Eigen::Index i, Eigen::Index j) {
    if (value == 1) {
      finds.emplace_back(i, j);
    }
  }
  // called for all other coefficients
  void operator()(i32 value, Eigen::Index i, Eigen::Index j) {
    if (value == 1) {
      finds.emplace_back(i, j);
    }
  }
};

std::tuple<IndexSet, IndexSet> ExpandSky(const Sky &sky) {
  IndexSet expandedRows;
  IndexSet expandedCols;
  for (int row = 0; row < sky.rows(); row++) {
    if (sky.row(row).isZero()) {
      expandedRows.insert(row);
    }
  }
  for (int col = 0; col < sky.cols(); col++) {
    if (sky.col(col).isZero()) {
      expandedCols.insert(col);
    }
  }
  return {expandedRows, expandedCols};
}

u64 FindPairDistances(const IndexSet &expanded_rows,
                      const IndexSet &expanded_cols,
                      const std::vector<Galaxy> &galaxies,
                      u64 expansion_factor) {
  u64 result = 0;
  i32 i2, i1, j2, j1;
  for (int i = 0; i < galaxies.size(); i++) {
    for (int j = i + 1; j < galaxies.size(); j++) {
      if (galaxies[i].first > galaxies[j].first) {
        i2 = galaxies[i].first;
        i1 = galaxies[j].first;
      } else {
        i1 = galaxies[i].first;
        i2 = galaxies[j].first;
      }
      if (galaxies[i].second > galaxies[j].second) {
        j2 = galaxies[i].second;
        j1 = galaxies[j].second;
      } else {
        j1 = galaxies[i].second;
        j2 = galaxies[j].second;
      }
      result += i2 - i1 + j2 - j1;
      u64 empty_rows = expanded_rows.upper_bound(i2) - expanded_rows.lower_bound(i1);
      u64 empty_cols = expanded_cols.upper_bound(j2) - expanded_cols.lower_bound(j1);
      result += expansion_factor * (empty_rows + empty_cols);
    }
  }
  return result;
}

}  // namespace

template<>
auto advent<2023, 11>::solve() -> Result {
  std::string input = GetInput();
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  u32 height = lines.size();
  u32 width = lines.at(0).size();
  Sky sky{};
  sky.resize(height, width);
  for (u32 row = 0; row < height; row++) {
    for (u32 col = 0; col < width; col++) {
      sky(row, col) = lines.at(row).at(col) == '#' ? 1 : 0;
    }
  }

  // Part 1
  auto [empty_rows, empty_cols] = ExpandSky(sky);
  GalaxyFinder finder;
  sky.visit(finder);
  u64 part1 = FindPairDistances(empty_rows, empty_cols, finder.finds, 1);

  // Part 2
  u64 part2 = FindPairDistances(empty_rows, empty_cols, finder.finds, 999'999ll);

  return aoc::result(part1, part2);
}