#ifndef AOC_BENCHMARKS_H
#define AOC_BENCHMARKS_H

#include <benchmark/benchmark.h>
#include <aoc.hpp>

template <int YEAR, int DAY>
void AocBenchmarks(benchmark::State& state) {
  advent<YEAR, DAY> adv;
  adv.GetInput();
  for (auto _ : state) {
    benchmark::DoNotOptimize(adv.solve());
  }
}

#ifndef AOC_BENCHMARK
#define AOC_BENCHMARK(year, day) BENCHMARK(AocBenchmarks<(year), (day)>)\
    ->Name("AoC-" #year "-" #day)\
    ->Unit(benchmark::kMicrosecond)\
    ->UseRealTime()
#endif // AOC_BENCHMARK

#endif // AOC_BENCHMARKS_H
