#ifndef AOC_TESTS_H
#define AOC_TESTS_H

#include <aoc.hpp>
#include <gtest/gtest.h>

template <typename T>
class AoCTests : public testing::Test {};

TYPED_TEST_SUITE_P(AoCTests);

TYPED_TEST_P(AoCTests, ValidateAnswersTest) {
    static constexpr std::size_t year = TypeParam::first_type::value;
    static constexpr std::size_t day = TypeParam::second_type::value;
    advent<year, day> adv;
    const auto result = adv.DoSolve();
    EXPECT_EQ(std::get<0>(result), adv.PartOne());
    EXPECT_EQ(std::get<1>(result), adv.PartTwo());
}

REGISTER_TYPED_TEST_SUITE_P(AoCTests, ValidateAnswersTest);

#ifndef TEST_YEAR_DAY
#define TEST_YEAR_DAY(year, day) std::pair<std::integral_constant<std::size_t, (year)>, std::integral_constant<std::size_t, (day)>>
#endif // TEST_YEAR_DAY

#endif // AOC_TESTS_H
