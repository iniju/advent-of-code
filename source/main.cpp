#include <aoc.hpp>

auto main() -> int {
  constexpr int YEAR{2024};
  auto start = std::chrono::high_resolution_clock::now();

  advent<YEAR, 1>().print();
  advent<YEAR, 2>().print();
  advent<YEAR, 3>().print();
  advent<YEAR, 4>().print();
  advent<YEAR, 5>().print();
  advent<YEAR, 6>().print();
  advent<YEAR, 7>().print();
  advent<YEAR, 8>().print();
  advent<YEAR, 9>().print();
  advent<YEAR, 10>().print();
  advent<YEAR, 11>().print();
//  advent<YEAR, 12>().print();
//  advent<YEAR, 13>().print();
//  advent<YEAR, 14>().print();
//  advent<YEAR, 15>().print();
//  advent<YEAR, 16>().print();
//  advent<YEAR, 17>().print();
//  advent<YEAR, 18>().print();
//  advent<YEAR, 19>().print();
//  advent<YEAR, 20>().print();
//  advent<YEAR, 21>().print();
//  advent<YEAR, 22>().print();
//  advent<YEAR, 23>().print();
//  advent<YEAR, 24>().print();
//  advent<YEAR, 25>().print();

  auto end = std::chrono::high_resolution_clock::now();
  auto duration_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  fmt::print("Year {} total: {:0.5f} ms\n", YEAR, (long double) duration_nanos / 1.0e6l);
  return 0;
}