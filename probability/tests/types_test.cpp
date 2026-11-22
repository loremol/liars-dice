#include <liars_dice/probability.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
using liars_dice::probability::Bid;
using liars_dice::probability::Face;

TEST(Face, KeepsItsValue) {
    for (int value = 1; value <= 6; ++value) {
        EXPECT_EQ(Face{value}.value(), value);
    }
}
TEST(Face, OutOfRangeThrows) {
    EXPECT_THROW(Face{0}, std::invalid_argument);
    EXPECT_THROW(Face{7}, std::invalid_argument);
}
TEST(Face, OrdersByValue) {
    EXPECT_LT(Face{2}, Face{5});
    EXPECT_EQ(Face{4}, Face{4});
    EXPECT_NE(Face{4}, Face{3});
}
TEST(Bid, KeepsQuantityAndFace) {
    const Bid bid{3, 4};
    EXPECT_EQ(bid.quantity(), 3);
    EXPECT_EQ(bid.face(), Face{4});
}
TEST(Bid, InvalidQuantityOrFaceThrows) {
    EXPECT_THROW((Bid{0, 4}), std::invalid_argument);
    EXPECT_THROW((Bid{1, 0}), std::invalid_argument);
    EXPECT_THROW((Bid{1, 7}), std::invalid_argument);
}
