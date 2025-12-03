#include "aoc_tests.h"

using test_2025_types = ::testing::Types<
    TEST_YEAR_DAY(2025, 1),
    TEST_YEAR_DAY(2025, 2),
    TEST_YEAR_DAY(2025, 3)
>;

INSTANTIATE_TYPED_TEST_SUITE_P(My, AoCTests, test_2025_types);