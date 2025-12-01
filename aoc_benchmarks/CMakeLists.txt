project(AoC_benchmarks)

set(BENCHMARK_ENABLE_GTEST_TESTS FALSE)
include(FetchContent)
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG v1.9.1
)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(
        aoc_benchmarks
        aoc_benchmarks.h
        aoc_2025_benchmarks.cc
)
target_link_libraries(
        aoc_benchmarks
        aoc_lib
)
target_link_libraries(
        aoc_benchmarks
        benchmark::benchmark
        benchmark::benchmark_main
)
