#include <liars_dice/probability.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
using liars_dice::probability::isRaise;

TEST(IsRaise, AfterThreeFours) {
    EXPECT_TRUE(isRaise({4, 2}, {3, 4}));
    EXPECT_TRUE(isRaise({3, 5}, {3, 4}));
    EXPECT_TRUE(isRaise({4, 6}, {3, 4}));
    EXPECT_FALSE(isRaise({2, 6}, {3, 4}));
    EXPECT_FALSE(isRaise({3, 3}, {3, 4}));
}
TEST(IsRaise, EqualBidIsNotARaise) { EXPECT_FALSE(isRaise({3, 4}, {3, 4})); }
TEST(IsRaise, OnesRankLowest) {
    EXPECT_TRUE(isRaise({4, 1}, {3, 6}));
    EXPECT_FALSE(isRaise({3, 1}, {3, 2}));
}
TEST(IsRaise, InvalidBidsThrow) {
    EXPECT_THROW(isRaise({0, 4}, {3, 4}), std::invalid_argument);
    EXPECT_THROW(isRaise({3, 0}, {3, 4}), std::invalid_argument);
    EXPECT_THROW(isRaise({3, 7}, {3, 4}), std::invalid_argument);
    EXPECT_THROW(isRaise({3, 4}, {0, 4}), std::invalid_argument);
    EXPECT_THROW(isRaise({3, 4}, {3, 7}), std::invalid_argument);
}
