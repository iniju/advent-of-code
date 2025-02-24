#include <aoc.hpp>

#include <absl/container/btree_set.h>
#include <Eigen/Core>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace {

struct Brick {
  aoc::Pos xy1;
  aoc::Pos xy2;
  i32 bottom{0};
  i32 top{0};
};
using IdSet = absl::btree_set<i32>;
using Bricks = std::vector<Brick>;
using HeightMap = Eigen::MatrixXi;
Eigen::IOFormat CleanFmt(0, 0, " ", "\n");
using BottomAndId = std::tuple<i32, i32>;
using Supporters = absl::flat_hash_map<i32, IdSet>;
struct RestingIdsFinder {
  i32 resting;
  aoc::Pos offset;
  const HeightMap &ids;
  IdSet result;
  RestingIdsFinder(i32 _resting, const aoc::Pos &_offset, const HeightMap &ids_map)
      : resting(_resting), offset(_offset), ids(ids_map), result{} {};
  void init(i32 value, Eigen::Index i, Eigen::Index j) {
    if (value == resting) {
      i32 id = ids(i + offset.i, j + offset.j);
      if (id == -1) return;
      result.insert(id);
    }
  }
  void operator()(i32 value, Eigen::Index i, Eigen::Index j) {
    init(value, i, j);
  }
};

}  // namespace

namespace fmt {

template<>
struct formatter<Brick> : formatter<string_view> {
  auto format(const Brick &b, format_context &ctx) const {
    return formatter<string_view>::format(
        fmt::format("{},{},{}~{},{},{}", b.xy1.i, b.xy1.j, b.bottom, b.xy2.i, b.xy2.j, b.top),
        ctx);
  }
};
template<>
struct formatter<Eigen::WithFormat<HeightMap>> : ostream_formatter {};

}  // namespace fmt

namespace {

std::tuple<i32, i32> GetMapDimensions(const Bricks &bricks) {
  i64 min_i{1'000'000};
  i64 max_i{0};
  i64 min_j{1'000'000};
  i64 max_j{0};
  for (const auto &b : bricks) {
    if (b.bottom > b.top || b.xy1.i > b.xy2.i || b.xy1.j > b.xy2.j) fmt::print("{}\n", b);
    if (min_i > b.xy1.i) min_i = b.xy1.i;
    if (max_i < b.xy2.i) max_i = b.xy2.i;
    if (min_j > b.xy1.j) min_j = b.xy1.j;
    if (max_j < b.xy2.j) max_j = b.xy2.j;
  }
  CHECK(min_i == 0 && min_j == 0) << "Wrong assumption about start of map [" << min_i << ", " << min_j << "].";
  return {max_i + 1, max_j + 1};
}

std::tuple<Supporters, Supporters> DropBricks(Bricks &bricks, HeightMap &map) {
  Supporters supporters;
  Supporters supported_by;
  HeightMap ids;
  ids.resize(map.rows(), map.cols());
  ids.setConstant(-1);
  std::priority_queue<BottomAndId, std::vector<BottomAndId>, std::greater<>> pq;
  for (int bi = 0; bi < bricks.size(); bi++) pq.emplace(bricks.at(bi).bottom, bi);
  while (!pq.empty()) {
    auto [bottom, id] = pq.top();
    pq.pop();
    auto &brick = bricks[id];
    auto footprint = map(Eigen::seq(brick.xy1.i, brick.xy2.i), Eigen::seq(brick.xy1.j, brick.xy2.j));
    auto resting = footprint.maxCoeff() + 1;
    CHECK(resting <= brick.bottom) << fmt::format("No space to fall for {} at {}.", brick, resting);
    brick.top -= bottom - resting;
    brick.bottom = resting;
    RestingIdsFinder finder{resting - 1, brick.xy1, ids};
    footprint.visit(finder);
    for (i32 supporter : finder.result) {
      supporters[supporter].insert(id);
      supported_by[id].insert(supporter);
    }
    footprint.setConstant(brick.top);
    ids(Eigen::seq(brick.xy1.i, brick.xy2.i), Eigen::seq(brick.xy1.j, brick.xy2.j)).setConstant(id);
  }
  return {supporters, supported_by};
}

IdSet FindTransitiveSupporting(i32 id,
                             const Supporters &supporting,
                             const Supporters &supported_by) {
  IdSet falling{};
  if (!supporting.contains(id)) return falling;
  falling.insert(id);
  IdSet candidates{supporting.at(id)};
  IdSet new_candidates{};
  while (!candidates.empty()) {
    new_candidates.clear();
    for (i32 candidate : candidates) {
      IdSet diff{};
      absl::c_set_difference(supported_by.at(candidate), falling, std::inserter(diff, diff.begin()));
      if (diff.empty()) {
        falling.insert(candidate);
        if (supporting.contains(candidate)) {
          for (i32 new_cand : supporting.at(candidate)) new_candidates.insert(new_cand);
        }
      }
    }
    std::swap(candidates, new_candidates);
  }
  falling.erase(id);
  return falling;
}

}  // namespace


template<>
auto advent<2023, 22>::solve() -> Result {
  std::string input = GetInput();
  Bricks bricks = aoc::util::TokenizeInput<Brick>(input, [](auto line) {
    Brick b;
    auto sep = line.find('~');
    CHECK(sep != absl::string_view::npos) << "No '~' found in '" << line << "'.";
    CHECK(scn::scan(line.substr(0, sep), "{},{},{}", b.xy1.i, b.xy1.j, b.bottom));
    CHECK(scn::scan(line.substr(sep + 1), "{},{},{}", b.xy2.i, b.xy2.j, b.top));
    return b;
  });

  auto [height, width] = GetMapDimensions(bricks);
  HeightMap map;
  map.resize(height, width);
  map.setZero();

  // Part 1
  IdSet removable{};
  auto [supporters, supported_by] = DropBricks(bricks, map);
  for (i32 id = 0; id < bricks.size(); id++) {
    if (!supporters.contains(id)) {
      removable.insert(id);
      continue;
    }
    bool can_eliminate = true;
    for (i32 supportee : supporters.at(id)) {
      if (supported_by.at(supportee).size() == 1) {
        can_eliminate = false;
        break;
      }
    }
    if (can_eliminate) removable.insert(id);
  }
  u64 part1 = removable.size();

  // Part 2
  u64 part2 = 0;
  for (i32 id = 0; id < bricks.size(); id++) {
    part2 += FindTransitiveSupporting(id, supporters, supported_by).size();
  }

    return aoc::result(part1, part2);
}