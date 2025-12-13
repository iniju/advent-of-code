#include "aoc_tests.h"

using test_2025_types = ::testing::Types<
    TEST_YEAR_DAY(2025, 1),
    TEST_YEAR_DAY(2025, 2),
    TEST_YEAR_DAY(2025, 3),
    TEST_YEAR_DAY(2025, 4),
    TEST_YEAR_DAY(2025, 5),
    TEST_YEAR_DAY(2025, 6),
    TEST_YEAR_DAY(2025, 7),
    TEST_YEAR_DAY(2025, 8)
    TEST_YEAR_DAY(2025, 8),
    TEST_YEAR_DAY(2025, 9)
    TEST_YEAR_DAY(2025, 9),
    TEST_YEAR_DAY(2025, 10)
>;

INSTANTIATE_TYPED_TEST_SUITE_P(My, AoCTests, test_2025_types);