cmake_minimum_required(VERSION 3.9)
project(AoC)

set(CMAKE_BUILD_FLAGS "-O3 -PARALLEL")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(aoc_lib OBJECT
        aoc_lib/aoc.cpp
        aoc_lib/aoc.h
#        aoc_lib/2020/01/solver-2020-01.cpp
#        aoc_lib/2023/01/solver-2023-01.cpp
#        aoc_lib/2023/02/solver-2023-02.cpp
#        aoc_lib/2023/03/solver-2023-03.cpp
#        aoc_lib/2023/04/solver-2023-04.cpp
#        aoc_lib/2023/05/solver-2023-05.cpp
#        aoc_lib/2023/06/solver-2023-06.cpp
#        aoc_lib/2023/07/solver-2023-07.cpp
#        aoc_lib/2023/08/solver-2023-08.cpp
#        aoc_lib/2023/09/solver-2023-09.cpp
#        aoc_lib/2023/10/solver-2023-10.cpp
#        aoc_lib/2023/11/solver-2023-11.cpp
#        aoc_lib/2023/12/solver-2023-12.cpp
#        aoc_lib/2023/13/solver-2023-13.cpp
#        aoc_lib/2023/14/solver-2023-14.cpp
#        aoc_lib/2023/15/solver-2023-15.cpp
#        aoc_lib/2023/16/solver-2023-16.cpp
#        aoc_lib/2023/17/solver-2023-17.cpp
#        aoc_lib/2023/18/solver-2023-18.cpp
#        aoc_lib/2023/19/solver-2023-19.cpp
#        aoc_lib/2023/20/solver-2023-20.cpp
#        aoc_lib/2023/21/solver-2023-21.cpp
#        aoc_lib/2023/22/solver-2023-22.cpp
#        aoc_lib/2023/23/solver-2023-23.cpp
#        aoc_lib/2023/24/solver-2023-24.cpp
#        aoc_lib/2023/25/solver-2023-25.cpp

        aoc_lib/2024/01/solver-2024-01.cpp
#        aoc_lib/2024/02/solver-2024-02.cpp
#        aoc_lib/2024/03/solver-2024-03.cpp
#        aoc_lib/2024/04/solver-2024-04.cpp
#        aoc_lib/2024/05/solver-2024-05.cpp
#        aoc_lib/2024/06/solver-2024-06.cpp
#        aoc_lib/2024/07/solver-2024-07.cpp
#        aoc_lib/2024/08/solver-2024-08.cpp
#        aoc_lib/2024/09/solver-2024-09.cpp
#        aoc_lib/2024/10/solver-2024-10.cpp
#        aoc_lib/2024/11/solver-2024-11.cpp
#        aoc_lib/2024/12/solver-2024-12.cpp
#        aoc_lib/2024/13/solver-2024-13.cpp
#        aoc_lib/2024/14/solver-2024-14.cpp
#        aoc_lib/2024/15/solver-2024-15.cpp
#        aoc_lib/2024/16/solver-2024-16.cpp
#        aoc_lib/2024/17/solver-2024-17.cpp
#        aoc_lib/2024/18/solver-2024-18.cpp
#        aoc_lib/2024/19/solver-2024-19.cpp
#        aoc_lib/2024/20/solver-2024-20.cpp
#        aoc_lib/2024/21/solver-2024-21.cpp
#        aoc_lib/2024/22/solver-2024-22.cpp
#        aoc_lib/2024/23/solver-2024-23.cpp
#        aoc_lib/2024/24/solver-2024-24.cpp
#        aoc_lib/2024/25/solver-2024-25.cpp

        aoc_lib/2025/01/solver-2025-01.cpp
        aoc_lib/2025/02/solver-2025-02.cpp
        aoc_lib/2025/03/solver-2025-03.cpp
        aoc_lib/2025/04/solver-2025-04.cpp
        aoc_lib/2025/05/solver-2025-05.cpp
)

target_include_directories(aoc_lib PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/aoc_lib>"
)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(ABSL_ENABLE_INSTALL ON)
add_subdirectory(deps/abseil-cpp)
add_subdirectory(deps/re2)
add_compile_definitions(FMT_EXCEPTIONS=0)
add_subdirectory(deps/fmt)
set(CPP-LAZY_USE_STANDALONE TRUE)
add_subdirectory(deps/cpp-lazy)
add_subdirectory(deps/eigen)
set(SCN_REGEX_BACKEND "re2")
add_subdirectory(deps/scnlib)
find_package(OpenMP REQUIRED)

target_link_libraries(aoc_lib PUBLIC
        absl::btree
        absl::flat_hash_map
        absl::flat_hash_set
        absl::hash
        absl::log
        absl::strings
        fast_float
        fmt::fmt
        scn::scn
        Eigen3::Eigen
        re2::re2
        OpenMP::OpenMP_CXX
)

add_executable(AoC main.cpp)
target_link_libraries(AoC PRIVATE aoc_lib)

add_subdirectory(aoc_benchmarks)
add_subdirectory(aoc_tests)