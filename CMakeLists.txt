cmake_minimum_required(VERSION 3.26)
project(AoC)

set(CMAKE_CXX_STANDARD 20)

add_library(aoc_lib OBJECT
        source/aoc.cpp

        source/2020/01/solver.cpp
        source/2023/01/solver.cpp
        source/2023/02/solver.cpp
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
#set(CMAKE_PREFIX_PATH "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/libs")
# re2 hack
add_subdirectory(libs)

target_link_libraries(aoc_lib PUBLIC
        absl::base
        absl::core_headers
        absl::flat_hash_map
        absl::flat_hash_set
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
