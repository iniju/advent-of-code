#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_map.h>
#include <Eigen/Dense>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>

namespace {

using Platform = Eigen::MatrixXi;
Eigen::IOFormat PlatformFormat(Eigen::StreamPrecision, 0, "", "\n");
using Split = std::pair<u32, u32>;
using Splits = std::vector<Split>;

std::vector<std::pair<u32, u32>> SplitColByCube(const Platform &col) {
  std::vector<std::pair<u32, u32>> result{};
  i32 last_cube = -1;
  for (i32 i = 0; i < col.rows(); i++) {
    if (col(i) == 1) {
      if (i - last_cube > 2) {
        result.emplace_back(last_cube + 1, i - 1);
      }
      last_cube = i;
    }
  }
  if (last_cube < col.rows() - 2) result.emplace_back(last_cube + 1, col.rows() - 1);
  return result;
}
Splits SplitRowByCube(const Platform &row) {
  Splits result{};
  i32 last_cube = -1;
  for (i32 j = 0; j < row.cols(); j++) {
    if (row(j) == 1) {
      if (j - last_cube > 2) {
        result.emplace_back(last_cube + 1, j - 1);
      }
      last_cube = j;
    }
  }
  if (last_cube < row.cols() - 2) result.emplace_back(last_cube + 1, row.cols() - 1);
  return result;
}

void RollNorthSouth(Platform &round, const absl::btree_map<i32, Splits> &splits_by_col, bool roll_north) {
  for (i32 j = 0; j < round.cols(); j++) {
    for (auto split : splits_by_col.at(j)) {
      u32 round_stones = round(Eigen::seq(split.first, split.second), j).sum();
      if (round_stones == 0 || round_stones == split.second - split.first + 1) continue;
      if (roll_north) {
        round(Eigen::seq(split.first, split.first + round_stones - 1), j).setOnes();
        round(Eigen::seq(split.first + round_stones, split.second), j).setZero();
      } else {
        u32 round_blanks = 1 + split.second - split.first - round_stones;
        round(Eigen::seq(split.first, split.first + round_blanks - 1), j).setZero();
        round(Eigen::seq(split.first + round_blanks, split.second), j).setOnes();
      }
    }
  }
}

void RollEastWest(Platform &round, const absl::btree_map<i32, Splits> &splits_by_row, bool roll_west) {
  for (i32 i = 0; i < round.rows(); i++) {
    for (auto split : splits_by_row.at(i)) {
      u32 round_stones = round(i, Eigen::seq(split.first, split.second)).sum();
      if (round_stones == 0 || round_stones == split.second - split.first + 1) continue;
      if (roll_west) {
        round(i, Eigen::seq(split.first, split.first + round_stones - 1)).setOnes();
        round(i, Eigen::seq(split.first + round_stones, split.second)).setZero();
      } else {
        u32 round_blanks = 1 + split.second - split.first - round_stones;
        round(i, Eigen::seq(split.first, split.first + round_blanks - 1)).setZero();
        round(i, Eigen::seq(split.first + round_blanks, split.second)).setOnes();
      }
    }
  }
}

u64 CalculateLoad(const Platform &round) {
  u64 result{0};
  u64 row_load = round.rows();
  for (i32 i = 0; i < round.rows(); i++) {
    result += round.row(i).sum() * row_load--;
  }
  return result;
}

}  // namespace

namespace fmt {

template<>
struct formatter<Eigen::WithFormat<Platform>> : ostream_formatter {};

}  // namespace fmt

template<>
auto advent<2023, 14>::solve() -> Result {
  std::string input = GetInput(false);
  std::vector<absl::string_view> lines = absl::StrSplit(input, "\n", absl::SkipWhitespace());
  Platform round(lines.size(), lines.at(0).size());
  Platform cubic(lines.size(), lines.at(0).size());
  for (u32 i = 0; i < lines.size(); i++) {
    for (u32 j = 0; j < lines.at(i).size(); j++) {
      round(i, j) = lines.at(i).at(j) == 'O' ? 1 : 0;
      cubic(i, j) = lines.at(i).at(j) == '#' ? 1 : 0;
    }
  }

  // Split by rows and cols
  absl::btree_map<i32, Splits> splits_by_col{};
  absl::btree_map<i32, Splits> splits_by_row{};
  for (i32 j = 0; j < round.cols(); j++) {
    splits_by_col[j] = SplitColByCube(cubic.col(j));
  }
  for (i32 i = 0; i < round.rows(); i++) {
    splits_by_row[i] = SplitRowByCube(cubic.row(i));
  }

  // Part 1
  RollNorthSouth(round, splits_by_col, true);
  u64 part1 = CalculateLoad(round);

  // Part 2
  absl::flat_hash_map<size_t, u64> cache{};
  aoc::util::EigenMatrixHasher hash;
  // Complete first cycle.
  RollEastWest(round, splits_by_row, true);
  RollNorthSouth(round, splits_by_col, false);
  RollEastWest(round, splits_by_row, false);
  // cache[std::hash<Platform>{}(round)] = 1;
  cache[hash(aoc::util::EigenMatrixHashWrapper{round})] = 1;

  u64 remaining = 0;
  constexpr u64 N = 1'000'000'000;
  for (u64 i = 2; i <= N; i++) {
    RollNorthSouth(round, splits_by_col, true);
    RollEastWest(round, splits_by_row, true);
    RollNorthSouth(round, splits_by_col, false);
    RollEastWest(round, splits_by_row, false);
    // auto h = std::hash<Platform>{}(round);
    auto h = hash(aoc::util::EigenMatrixHashWrapper{round});
    if (cache.contains(h)) {
      u64 start_cycle = cache.at(h);
      u64 cycle = i - start_cycle;
      remaining = (N - start_cycle) % cycle;
      break;
    }
    cache[h] = i;
  }
  for (u64 i = 0; i < remaining; i++) {
    RollNorthSouth(round, splits_by_col, true);
    RollEastWest(round, splits_by_row, true);
    RollNorthSouth(round, splits_by_col, false);
    RollEastWest(round, splits_by_row, false);
  }
  u64 part2 = CalculateLoad(round);

  return aoc::result(part1, part2);
}