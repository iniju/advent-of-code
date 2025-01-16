#include <aoc.hpp>

#include <absl/container/btree_map.h>
#include <fmt/format.h>

namespace {

using Block = struct {
  u64 id;
  bool free;
  u64 size;
};
using Disk = std::vector<Block>;
using Blocks = std::vector<std::vector<Block>>;

void PrintDisk(const Disk &disk) {
  for (const auto &block : disk) {
    for (u64 i = 0; i < block.size; i++) {
      fmt::print("{:c}", block.free ? '.' : '0' + block.id);
    }
  }
  fmt::print("\n");
  fflush(stdout);
}
void PrintDisk(const Blocks &blocks) {
  u64 rows = 0;
  for (const auto &block : blocks) {
    for (const auto &b : block) {
      for (u64 i = 0; i < b.size; i++) {
        fmt::print("{:c}", b.free ? '.' : '0' + b.id);
      }
    }
    fmt::print("\n");
    rows++;
    if (rows > 10) break;
  }
  fmt::print("\n");
  fflush(stdout);
}

u64 CalcCheckSum(const Disk &disk) {
  u64 result = 0;
  u64 pos = 0;
  for (const auto &block : disk) {
    if (!block.free) {
      result += block.id * (block.size * pos + (block.size * (block.size - 1) / 2));
    }
    pos += block.size;
  }
  return result;
}

u64 CalcCheckSum(const Blocks &blocks) {
  u64 result = 0;
  u64 pos = 0;
  for (const auto &block : blocks) {
    for (const auto &b : block) {
      if (!b.free) {
        result += b.id * (b.size * pos + (b.size * (b.size - 1) / 2));
      }
      pos += b.size;
    }
  }
  return result;
}

}  // namespace

namespace fmt {

}  // namespace fmt

template<>
auto advent<2024, 9>::solve() -> Result {
  std::string input = GetInput();

  Disk disk;
  disk.reserve(input.size());
  bool is_file = true;
  u64 id = 0;
  for (char ch : input) {
    disk.emplace_back(id, !is_file, ch - '0');
    id += is_file;
    is_file = !is_file;
  }

  // Part 1
  Disk disk1{disk};
  u64 next_free = 1, next_file = disk1.size() - 1;
  while (next_free < next_file) {
    if (disk1[next_free].size <= disk1[next_file].size) {
      disk1[next_free].id = disk1[next_file].id;
      disk1[next_free].free = false;
      if (disk1[next_free].size == disk1[next_file].size) {
        disk1[next_file].free = true;
        next_file -= 2;
      } else {
        disk1[next_file].size -= disk1[next_free].size;
        if (next_file == disk1.size() - 1) disk1.emplace_back(0, true, disk1[next_free].size);
        else
          disk1[next_file + 1].size += disk1[next_free].size;
      }
      next_free += 2;
    } else if (disk1[next_free].size > disk1[next_file].size) {
      Block new_file(disk1[next_file]);
      disk1[next_free].size -= disk1[next_file].size;
      disk1[next_file].free = true;
      disk1.insert(disk1.begin() + static_cast<i64>(next_free), new_file);
      next_free++;
      next_file--;
    }
  }
  u64 part1 = CalcCheckSum(disk1);

  // Part 2
  Blocks blocks;
  blocks.reserve(disk.size());
  for (const auto &block : disk) blocks.push_back({block});

  next_free = 1;
  for (u64 file_i = disk.size() - 1; file_i > 1; file_i -= 2) {
    auto &file_block = blocks[file_i].front();
    CHECK(blocks.at(file_i).size() == 1) << "This block position should only have a single block.";
    CHECK(!file_block.free) << "This block should be file.";
    // Find first free block, of any size.
    while (blocks.at(next_free).back().size == 0) {
      CHECK(blocks.at(next_free).back().free) << "This block should be free.";
      next_free += 2;
    }
    // Find first free block that fits file.
    u64 free_i = next_free;
    while (free_i < file_i && file_block.size > blocks.at(free_i).back().size) {
      free_i += 2;
    }
    if (free_i >= file_i) continue;  // No move, next_file.
    // Move
    u64 remaining_size = blocks.at(free_i).back().size - file_block.size;
    blocks[free_i].back().id = file_block.id;
    blocks[free_i].back().free = false;
    blocks[free_i].back().size = file_block.size;
    blocks[free_i].emplace_back(0, true, remaining_size);
    file_block.id = 0;
    file_block.free = true;
  }
  u64 part2 = CalcCheckSum(blocks);

  return aoc::result(part1, part2);
}