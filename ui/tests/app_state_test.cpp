#include <gtest/gtest.h>

#include <liars_dice/probability.hpp>

#include "app_state.hpp"

using liars_dice::probability::Bid;

TEST(ClampPreviousBid, QuantityAboveTotalDropsToTotal) {
    AppState state;
    state.simpleMode = false;
    state.previousBid = Bid{30, 4};
    state.clampPreviousBid();
    EXPECT_EQ(state.previousBid, (Bid{15, 4}));
}

TEST(ClampPreviousBid, QuantityInRangeIsUnchanged) {
    AppState state;
    state.simpleMode = false;
    state.previousBid = Bid{7, 4};
    state.clampPreviousBid();
    EXPECT_EQ(state.previousBid, (Bid{7, 4}));
}

TEST(ClampPreviousBid, FollowsHiddenDiceGoingDown) {
    AppState state;
    state.simpleMode = false;
    state.previousBid = Bid{15, 4};
    state.hiddenDice = 0;
    state.clampPreviousBid();
    EXPECT_EQ(state.previousBid, (Bid{5, 4}));
}

TEST(ClampPreviousBid, SimpleModeClampsToHiddenDice) {
    AppState state;
    state.simpleMode = true;
    state.previousBid = Bid{15, 4};
    state.clampPreviousBid();
    EXPECT_EQ(state.previousBid, (Bid{10, 4}));
}

TEST(ClampPreviousBid, NoDiceAtAllKeepsQuantityOne) {
    AppState state;
    state.simpleMode = true;
    state.hiddenDice = 0;
    state.previousBid = Bid{3, 4};
    state.clampPreviousBid();
    EXPECT_EQ(state.previousBid, (Bid{1, 4}));
}
