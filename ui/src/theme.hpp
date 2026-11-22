#pragma once

#include <imgui.h>

// Muted violet-grey: clearly distinct from the teal/amber/red confidence ramp in
// legalRaiseBackground, since "this is the previous bid" isn't a confidence reading.
constexpr ImU32 kPreviousBidBackground = IM_COL32(120, 108, 138, 230);

// Confidence ramp: teal reads as an unlikely raise, amber as a toss-up, red as near-certain.
ImU32 legalRaiseBackground(double chanceTrue);

void applyLiarsDiceTheme();
