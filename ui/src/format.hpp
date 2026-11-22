#pragma once

#include <cmath>
#include <format>
#include <string>

#include <liars_dice/probability.hpp>

inline const char* faceText(liars_dice::probability::Face face) {
    static constexpr const char* kTexts[] = {"1", "2", "3", "4", "5", "6"};
    return kTexts[face.value() - 1];
}

inline const char* facePluralText(liars_dice::probability::Face face) {
    static constexpr const char* kTexts[] = {"1s", "2s", "3s", "4s", "5s", "6s"};
    return kTexts[face.value() - 1];
}

inline std::string formatPercent(double probability, double complementProbability, int decimals) {
    if (complementProbability == 0.0) return "100%";
    if (probability == 0.0) return "0%";
    const double scale = decimals == 0 ? 1.0 : 10.0;
    const double scaledPercent = std::round(probability * 100.0 * scale);
    if (scaledPercent >= 100.0 * scale) return decimals == 0 ? ">99%" : ">99.9%";
    if (scaledPercent <= 0.0) return decimals == 0 ? "<1%" : "<0.1%";
    return std::format("{:.{}f}%", scaledPercent / scale, decimals);
}

inline int legalRaiseAlpha(double chanceTrue) {
    return static_cast<int>(std::lround(64 + 191 * chanceTrue));
}

// Confidence ramp for the odds table: teal reads as an unlikely raise, amber as a toss-up,
// red as a near-certain one.
struct LegalRaiseColor {
    int red = 0;
    int green = 0;
    int blue = 0;
};

inline int lerpChannel(int from, int to, double t) {
    return static_cast<int>(std::lround(from + (to - from) * t));
}

inline LegalRaiseColor legalRaiseColor(double chanceTrue) {
    constexpr LegalRaiseColor kTeal{38, 166, 154};
    constexpr LegalRaiseColor kAmber{255, 179, 0};
    constexpr LegalRaiseColor kRed{168, 79, 63};
    if (chanceTrue <= 0.5) {
        const double t = chanceTrue / 0.5;
        return {lerpChannel(kTeal.red, kAmber.red, t), lerpChannel(kTeal.green, kAmber.green, t),
                lerpChannel(kTeal.blue, kAmber.blue, t)};
    }
    const double t = (chanceTrue - 0.5) / 0.5;
    return {lerpChannel(kAmber.red, kRed.red, t), lerpChannel(kAmber.green, kRed.green, t),
            lerpChannel(kAmber.blue, kRed.blue, t)};
}

inline std::string bidOddsTooltip(liars_dice::probability::Bid bid,
                                  const liars_dice::probability::BidOdds& bidOdds, bool simpleMode) {
    const int quantity = bid.quantity();
    const std::string title =
        simpleMode ? std::format("{} matching among the hidden dice", quantity)
                   : std::format("{} x {} on the table", quantity, facePluralText(bid.face()));
    return std::format("{}\nTrue ({} or more): {}\nExactly {}: {}\nLiar call wins (fewer than {}): {}",
                       title, quantity, formatPercent(bidOdds.atLeast, bidOdds.fewer, 1),
                       quantity, formatPercent(bidOdds.exactly, 1.0 - bidOdds.exactly, 1),
                       quantity, formatPercent(bidOdds.fewer, bidOdds.atLeast, 1));
}
