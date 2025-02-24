project(AoC_tests)

include(FetchContent)
FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP
        URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip # v1.16.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        aoc_tests
        aoc_tests.h
        aoc_2024_tests.cc
)
target_link_libraries(
        aoc_tests
        aoc_lib
)
target_link_libraries(
        aoc_tests
        gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(aoc_tests)