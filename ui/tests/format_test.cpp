#include <gtest/gtest.h>

#include <liars_dice/probability.hpp>

#include "format.hpp"

TEST(BidOddsTooltip, NamesTheBidAndAllThreeChances) {
    EXPECT_EQ(bidOddsTooltip({4, 5}, {0.213, 0.62, 0.38}, false),
              "4 x 5s on the table\n"
              "True (4 or more): 62.0%\n"
              "Exactly 4: 21.3%\n"
              "Liar call wins (fewer than 4): 38.0%");
}

TEST(BidOddsTooltip, SimpleModeCountsOnlyHiddenDice) {
    EXPECT_EQ(bidOddsTooltip({4, 5}, {0.213, 0.62, 0.38}, true),
              "4 matching among the hidden dice\n"
              "True (4 or more): 62.0%\n"
              "Exactly 4: 21.3%\n"
              "Liar call wins (fewer than 4): 38.0%");
}

TEST(BidOddsTooltip, OnlyCertaintyShowsWholePercentages) {
    EXPECT_EQ(bidOddsTooltip({1, 4}, {0.0, 1.0, 0.0}, false),
              "1 x 4s on the table\n"
              "True (1 or more): 100%\n"
              "Exactly 1: 0%\n"
              "Liar call wins (fewer than 1): 0%");
}

TEST(BidOddsTooltip, NearCertainNeverLooksCertain) {
    EXPECT_EQ(bidOddsTooltip({3, 2}, {0.00004, 0.99996, 0.00004}, false),
              "3 x 2s on the table\n"
              "True (3 or more): >99.9%\n"
              "Exactly 3: <0.1%\n"
              "Liar call wins (fewer than 3): <0.1%");
}

TEST(LegalRaiseAlpha, ImpossibleBidKeepsAQuarterTint) { EXPECT_EQ(legalRaiseAlpha(0.0), 64); }

TEST(LegalRaiseAlpha, CertainBidIsOpaque) { EXPECT_EQ(legalRaiseAlpha(1.0), 255); }

TEST(LegalRaiseAlpha, GrowsLinearlyWithTheChance) { EXPECT_EQ(legalRaiseAlpha(0.5), 160); }

TEST(LegalRaiseColor, ImpossibleBidIsTeal) {
    const LegalRaiseColor color = legalRaiseColor(0.0);
    EXPECT_EQ(color.red, 38);
    EXPECT_EQ(color.green, 166);
    EXPECT_EQ(color.blue, 154);
}

TEST(LegalRaiseColor, TossUpIsAmber) {
    const LegalRaiseColor color = legalRaiseColor(0.5);
    EXPECT_EQ(color.red, 255);
    EXPECT_EQ(color.green, 179);
    EXPECT_EQ(color.blue, 0);
}

TEST(LegalRaiseColor, CertainBidIsRed) {
    const LegalRaiseColor color = legalRaiseColor(1.0);
    EXPECT_EQ(color.red, 168);
    EXPECT_EQ(color.green, 79);
    EXPECT_EQ(color.blue, 63);
}
