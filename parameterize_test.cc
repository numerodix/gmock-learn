#include "_main.skel"

class AFixtureUsingInt : public ::testing::TestWithParam<int> {};

TEST_P(AFixtureUsingInt, InputIsLessThanTen) {
    int input = GetParam();  // is fed from the instantiation below
    ASSERT_LT(input, 10);
}

TEST_P(AFixtureUsingInt, InputIsGreaterThanZero) {
    int input = GetParam();
    ASSERT_GT(input, 0);
}

// Run every test that is part of AFixtureUsingInt with the values 1, 2, 9
INSTANTIATE_TEST_SUITE_P(
    RunOnNumbersBetweenZeroAndTen, AFixtureUsingInt, ::testing::Values(1, 2, 9)
);

// Run every test that is part of AFixtureUsingInt with the values 3, 4
INSTANTIATE_TEST_SUITE_P(
    RunOnNumbersBetweenThreeAndFour, AFixtureUsingInt, ::testing::Values(3, 4)
);


// TODO: add example of test parameterized by type, not by value