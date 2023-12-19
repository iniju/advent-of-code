cmake_minimum_required(VERSION 3.26)
project(AoC)

set(CMAKE_CXX_STANDARD 20)

add_library(aoc_lib OBJECT

        source/2020/01/solver-2020-01.cpp
        source/2023/01/solver-2023-01.cpp
        source/2023/02/solver-2023-02.cpp
        source/2023/03/solver-2023-03.cpp
        source/2023/04/solver-2023-04.cpp
        source/2023/05/solver-2023-05.cpp
        source/2023/06/solver-2023-06.cpp
        source/2023/07/solver-2023-07.cpp
        source/2023/08/solver-2023-08.cpp
        source/2023/09/solver-2023-09.cpp
        source/2023/10/solver-2023-10.cpp
        source/2023/11/solver-2023-11.cpp
        source/2023/12/solver-2023-12.cpp
        source/2023/13/solver-2023-13.cpp
        source/2023/14/solver-2023-14.cpp
        source/2023/15/solver-2023-15.cpp
        source/2023/16/solver-2023-16.cpp
        source/2023/17/solver-2023-17.cpp
        source/2023/18/solver-2023-18.cpp
        source/2023/19/solver-2023-19.cpp
)

target_include_directories(aoc_lib PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

add_subdirectory(libs/abseil-cpp)
add_compile_definitions(FMT_EXCEPTIONS=0)
add_subdirectory(libs/fmt)
add_subdirectory(libs/scnlib)
set(CPP-LAZY_USE_STANDALONE TRUE)
add_subdirectory(libs/cpp-lazy)
add_subdirectory(libs/eigen)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
# re2 cmake instructions are in libs/CMakeLists.txt
add_subdirectory(libs)

target_link_libraries(aoc_lib PUBLIC
        absl::btree
        absl::flat_hash_map
        absl::flat_hash_set
        absl::hash
        absl::log
        absl::strings
        fmt::fmt
        scn::scn
        cpp-lazy::cpp-lazy
        Eigen3::Eigen
        re2::re2
)

add_executable(AoC
        source/main.cpp
        source/aoc.hpp
)
target_link_libraries(AoC PRIVATE aoc_lib)
