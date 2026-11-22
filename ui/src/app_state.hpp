#pragma once

#include <algorithm>
#include <vector>

#include <liars_dice/probability.hpp>

struct AppState {
    bool simpleMode = true;
    std::vector<liars_dice::probability::Face> enteredDice{1, 1, 1, 1, 1};
    int hiddenDice = 10;
    bool opening = true;
    liars_dice::probability::Bid previousBid{1, 1};

    std::vector<liars_dice::probability::Face> knownDice() const {
        return simpleMode ? std::vector<liars_dice::probability::Face>{} : enteredDice;
    }

    int totalDice() const { return static_cast<int>(knownDice().size()) + hiddenDice; }

    void clampPreviousBid() {
        const int quantity = std::clamp(previousBid.quantity(), 1, std::max(1, totalDice()));
        previousBid = liars_dice::probability::Bid{quantity, previousBid.face()};
    }
};
