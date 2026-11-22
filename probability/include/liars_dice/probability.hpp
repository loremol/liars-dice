#pragma once
#include <stdexcept>
#include <vector>

namespace liars_dice::probability {

class Face {
public:
    constexpr Face(int value) : value_(value) {
        if (value < 1 || value > 6) {
            throw std::invalid_argument("face must be between 1 and 6");
        }
    }

    constexpr int value() const noexcept { return value_; }

    constexpr auto operator<=>(const Face&) const = default;

private:
    int value_;
};

class Bid {
public:
    constexpr Bid(int quantity, Face face) : quantity_(quantity), face_(face) {
        if (quantity < 1) {
            throw std::invalid_argument("quantity must be at least 1");
        }
    }

    constexpr int quantity() const noexcept { return quantity_; }
    constexpr Face face() const noexcept { return face_; }

    constexpr bool operator==(const Bid&) const = default;

private:
    int quantity_;
    Face face_;
};

struct BidOdds {
    double exactly;
    double atLeast;
    double fewer;
};

BidOdds odds(Bid bid, const std::vector<Face>& knownDice, int hiddenDice);

bool isRaise(Bid candidate, Bid previous);

}  // namespace liars_dice::probability
