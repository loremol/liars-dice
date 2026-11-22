#include <liars_dice/probability.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>
using liars_dice::probability::Bid;
using liars_dice::probability::BidOdds;
using liars_dice::probability::Face;
using liars_dice::probability::odds;

namespace {

void expectSameOdds(const BidOdds& actual, const BidOdds& expected) {
    EXPECT_NEAR(actual.exactly, expected.exactly, 1e-12);
    EXPECT_NEAR(actual.atLeast, expected.atLeast, 1e-12);
    EXPECT_NEAR(actual.fewer, expected.fewer, 1e-12);
}

}  // namespace

TEST(Odds, NoHiddenDiceIsDeterminedByOwnHand) {
    BidOdds certain = odds({2, 4}, {4, 4, 1}, 0);
    EXPECT_EQ(certain.exactly, 1.0);
    EXPECT_EQ(certain.atLeast, 1.0);
    EXPECT_EQ(certain.fewer, 0.0);

    BidOdds impossible = odds({3, 4}, {4, 4, 1}, 0);
    EXPECT_EQ(impossible.exactly, 0.0);
    EXPECT_EQ(impossible.atLeast, 0.0);
    EXPECT_EQ(impossible.fewer, 1.0);

    BidOdds overshoot = odds({1, 4}, {4, 4, 1}, 0);
    EXPECT_EQ(overshoot.exactly, 0.0);
    EXPECT_EQ(overshoot.atLeast, 1.0);
    EXPECT_EQ(overshoot.fewer, 0.0);
}

TEST(Odds, HoldingEnoughIsCertain) {
    BidOdds result = odds({2, 4}, {4, 4, 1, 6, 2}, 10);
    EXPECT_EQ(result.atLeast, 1.0);
    EXPECT_EQ(result.fewer, 0.0);
    EXPECT_NEAR(result.exactly, 0.1615055828898458, 1e-12);
}

TEST(Odds, MatchesBinomialWithNoOwnDice) {
    BidOdds result = odds({1, 3}, {}, 6);
    EXPECT_NEAR(result.atLeast, 0.6651020233196161, 1e-12);
    EXPECT_NEAR(result.exactly, 0.4018775720164610, 1e-12);
    EXPECT_NEAR(result.fewer, 0.3348979766803839, 1e-12);
}

TEST(Odds, OwnDiceShiftTheThreshold) {
    BidOdds result = odds({3, 4}, {4, 4, 1, 6, 2}, 10);
    EXPECT_NEAR(result.atLeast, 0.8384944171101545, 1e-12);
    EXPECT_NEAR(result.exactly, 0.3230111657796916, 1e-12);
    EXPECT_NEAR(result.fewer, 0.1615055828898455, 1e-12);
}

TEST(Odds, OnesAreNotWild) {
    expectSameOdds(odds({1, 1}, {}, 6), odds({1, 3}, {}, 6));
}

TEST(Odds, OtherFacesInHandDontCount) {
    expectSameOdds(odds({1, 3}, {4, 4, 1, 6, 2}, 6), odds({1, 3}, {}, 6));
}

TEST(Odds, QuantityAboveTotalIsImpossible) {
    BidOdds result = odds({16, 4}, {4, 4, 1, 6, 2}, 10);
    EXPECT_EQ(result.atLeast, 0.0);
    EXPECT_EQ(result.exactly, 0.0);
    EXPECT_EQ(result.fewer, 1.0);
}

TEST(Odds, LongShotsArePossibleNotZero) {
    BidOdds thirty = odds({30, 6}, {}, 35);
    EXPECT_GT(thirty.atLeast, 0.0);
    EXPECT_GT(thirty.exactly, 0.0);
    EXPECT_NEAR(thirty.atLeast, 6.096509673443193e-19,
                6.096509673443193e-19 * 1e-9);

    BidOdds thirtyFive = odds({35, 6}, {}, 35);
    EXPECT_GT(thirtyFive.atLeast, 0.0);
    EXPECT_NEAR(thirtyFive.atLeast, 5.81709607391588e-28,
                5.81709607391588e-28 * 1e-9);
}

TEST(Odds, InvariantsHoldOverSweep) {
    const std::vector<int> hiddenDiceCounts = {0, 1, 5, 10, 35};
    const std::vector<std::vector<Face>> hands = {
        {}, {4, 4, 1, 6, 2}, {6}, {1, 1, 1, 1, 1}};

    for (int hiddenDiceCount : hiddenDiceCounts) {
        for (const std::vector<Face>& hand : hands) {
            for (int face = 1; face <= 6; ++face) {
                double previousAtLeast = -1.0;
                int maximumQuantity =
                    static_cast<int>(hand.size()) + hiddenDiceCount + 1;
                for (int quantity = 1; quantity <= maximumQuantity; ++quantity) {
                    BidOdds result = odds({quantity, face}, hand, hiddenDiceCount);

                    EXPECT_GE(result.exactly, 0.0);
                    EXPECT_LE(result.exactly, 1.0);
                    EXPECT_GE(result.atLeast, 0.0);
                    EXPECT_LE(result.atLeast, 1.0);
                    EXPECT_GE(result.fewer, 0.0);
                    EXPECT_LE(result.fewer, 1.0);

                    EXPECT_NEAR(result.atLeast + result.fewer, 1.0, 1e-12);
                    EXPECT_LE(result.exactly, result.atLeast + 1e-12);

                    if (quantity > 1) {
                        EXPECT_LE(result.atLeast, previousAtLeast);
                    }
                    previousAtLeast = result.atLeast;
                }
            }
        }
    }
}

TEST(Odds, InvalidInputThrows) {
    EXPECT_THROW(odds({0, 4}, {}, 6), std::invalid_argument);
    EXPECT_THROW(odds({1, 0}, {}, 6), std::invalid_argument);
    EXPECT_THROW(odds({1, 7}, {}, 6), std::invalid_argument);
    EXPECT_THROW(odds({1, 4}, {}, -1), std::invalid_argument);
    EXPECT_THROW(odds({1, 4}, {0}, 6), std::invalid_argument);
    EXPECT_THROW(odds({1, 4}, {7}, 6), std::invalid_argument);
}
