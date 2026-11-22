#include <liars_dice/probability.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace liars_dice::probability {

namespace {

double binomialProbability(int matchingDice, int rolledDice) {
    double ways = 1.0;
    for (int factor = 1; factor <= matchingDice; ++factor) {
        ways = ways * (rolledDice - matchingDice + factor) / factor;
    }
    return ways * std::pow(1.0 / 6.0, matchingDice) *
           std::pow(5.0 / 6.0, rolledDice - matchingDice);
}

}  // namespace

BidOdds odds(Bid bid, const std::vector<Face>& knownDice, int hiddenDice) {
    if (hiddenDice < 0) {
        throw std::invalid_argument("hiddenDice must be non-negative");
    }

    int knownMatchingDice =
        static_cast<int>(std::count(knownDice.begin(), knownDice.end(), bid.face()));
    int hiddenMatchesNeeded = bid.quantity() - knownMatchingDice;

    double atLeast;
    double exactly;

    if (hiddenMatchesNeeded <= 0) {
        atLeast = 1.0;
        exactly = (hiddenMatchesNeeded == 0) ? binomialProbability(0, hiddenDice) : 0.0;
    } else if (hiddenMatchesNeeded > hiddenDice) {
        atLeast = 0.0;
        exactly = 0.0;
    } else {
        atLeast = 0.0;
        for (int hiddenMatches = hiddenDice; hiddenMatches >= hiddenMatchesNeeded;
             --hiddenMatches) {
            atLeast += binomialProbability(hiddenMatches, hiddenDice);
        }
        exactly = binomialProbability(hiddenMatchesNeeded, hiddenDice);
    }

    double fewer = 1.0 - atLeast;

    return BidOdds{exactly, atLeast, fewer};
}

}  // namespace liars_dice::probability
