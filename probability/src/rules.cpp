#include <liars_dice/probability.hpp>

namespace liars_dice::probability {

bool isRaise(Bid candidate, Bid previous) {
    if (candidate.quantity() != previous.quantity()) {
        return candidate.quantity() > previous.quantity();
    }
    return candidate.face() > previous.face();
}

}  // namespace liars_dice::probability
