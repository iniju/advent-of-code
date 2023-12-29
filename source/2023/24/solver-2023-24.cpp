#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <re2/re2.h>

namespace {

double zero = 1e-2;
using Vec3 = Eigen::Matrix<i64, 3, 1>;
Eigen::IOFormat Vec3Fmt(0, 0, ", ", ", ");
struct Hail {
  Vec3 pos;
  Vec3 vel;
  Hail() : pos{}, vel{} {
    pos.setZero();
    vel.setZero();
  }
};
inline long double i64_ld(i64 x) {
  return static_cast<long double>(x);
}

}  // namespace

namespace fmt {

template<>
struct formatter<Eigen::WithFormat<Vec3>> : ostream_formatter {};
template<>
struct formatter<Eigen::WithFormat<Eigen::Vector3f>> : ostream_formatter {};
template<>
struct formatter<Hail> : formatter<string_view> {
  auto format(const Hail &hs, format_context &ctx) const {
    return formatter<string_view>::format(
        fmt::format("{} @ {}", hs.pos.format(Vec3Fmt), hs.vel.format(Vec3Fmt)), ctx);
  }
};

}  // namespace fmt

namespace {

std::tuple<bool, long double, long double> Collide(const Hail &h1,
                                                   const Hail &h2) {
  long double px1 = i64_ld(h1.pos(0));
  long double py1 = i64_ld(h1.pos(1));
  long double px2 = i64_ld(h2.pos(0));
  long double py2 = i64_ld(h2.pos(1));
  long double ux1 = i64_ld(h1.vel(0));
  long double uy1 = i64_ld(h1.vel(1));
  long double ux2 = i64_ld(h2.vel(0));
  long double uy2 = i64_ld(h2.vel(1));
  long double denom = uy1 * ux2 - uy2 * ux1;
  if (abs(denom) < zero) {
    return {false, 0, 0};
  }
  long double c = py1 - uy1 * px1 / ux1;
  long double d = py2 - uy2 * px2 / ux2;
  long double px = (d - c) * ux1 * ux2 / denom;
  long double py = px * (uy1 / ux1) + c;
  long double t1 = (px - i64_ld(h1.pos(0))) / i64_ld(h1.vel(0));
  long double t2 = (px - i64_ld(h2.pos(0))) / i64_ld(h2.vel(0));
  if (t1 < 0 || t2 < 0) {
    return {false, 0, 0};
  }
  return {true, px, py};
}

}  // namespace

template<>
auto advent<2023, 24>::solve() -> Result {
  std::string input = GetInput(false);
  std::vector<Hail> hails = aoc::util::TokenizeInput<Hail>(input, [](auto line) {
    Hail hs;
    CHECK(scn::scan(line, "{}, {}, {} @ {}, {}, {}", hs.pos(0), hs.pos(1), hs.pos(2), hs.vel(0), hs.vel(1), hs.vel(2)))
            << fmt::format("Couldn't parse '{}'", line);
    CHECK(hs.pos(0) * hs.pos(1) * hs.pos(2) != 0) << fmt::format("Found zero speed {}", line);
    return hs;
  });

  // Part 1
  u64 part1 = 0;
  long double min = 200'000'000'000'000.0;
  long double max = 400'000'000'000'000.0;
  for (int i = 0; i < hails.size() - 1; i++) {
    for (int j = i + 1; j < hails.size(); j++) {
      const auto &h1 = hails.at(i);
      const auto &h2 = hails.at(j);
      auto [collide, px, py] = Collide(h1, h2);
      if (!collide) {
        continue;
      }
      if (px >= min && px <= max && py >= min && py <= max) {
        part1++;
      }
    }
  }

  // Part 2
  Hail stone;
  // Idea from https://www.reddit.com/r/adventofcode/comments/18pnycy/comment/kf6sopk/?utm_source=share&utm_medium=web2x&context=3
  // The solution (correctly) assumes that all collisions happen at integer nanosecond times.
  // Then it uses the distance (per axis) between hailstones with same speed on that axis, to narrow down the possible
  // integer speeds that can work for this axis. Turns out there's a single that works out per axis.
  for (i8 dim = 0; dim < 3; dim++) {
    absl::btree_map<i64, std::vector<i64>> same_speeds;
    for (const auto &h : hails) {
      same_speeds[h.vel(dim)].push_back(h.pos(dim));
    }
    absl::erase_if(same_speeds, [](const auto &kv) { return kv.second.size() < 2; });
    std::vector<std::pair<i64, i64>> same_speed_dists;
    for (const auto &[k, v] : same_speeds) {
      for (i32 i = 0; i < v.size() - 1; i++) {
        same_speed_dists.emplace_back(k, abs(v.at(i + 1) - v.at(i)));
      }
    }
    i64 speed_i = 0;
    while (speed_i < 2000) {
      i64 speed = (speed_i + 1) / 2 * (speed_i % 2 == 0 ? 1 : -1);
      if (absl::c_all_of(same_speed_dists,
                         [speed](const std::pair<i64, i64> &p) {
                           return (speed - p.first) != 0 && p.second % (speed - p.first) == 0;
                         })) {
        stone.vel(dim) = speed;
        break;
      }
      speed_i++;
    }
  }

  i64 u1x = stone.vel(0) - hails[0].vel(0);
  i64 u2x = stone.vel(0) - hails[1].vel(0);
  i64 u1y = stone.vel(1) - hails[0].vel(1);
  i64 u2y = stone.vel(1) - hails[1].vel(1);
  i64 denom = (u1x * u2y - u1y * u2x);
  CHECK(denom != 0) << "No solution exists at integer nanoseconds time.";
  i64 t = (u2x * (hails[1].pos(1) - hails[0].pos(1)) - (hails[1].pos(0) - hails[0].pos(0)) * u2y) / denom;
  stone.pos = hails[0].pos + (hails[0].vel - stone.vel) * t;
  u64 part2 = stone.pos.sum();

  return aoc::result(part1, part2);
}
