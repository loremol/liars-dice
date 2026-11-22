#include <cmath>

#include <gtest/gtest.h>

#include <imgui.h>

#include "app.hpp"
#include "display_fit.hpp"
#include "headless_imgui.hpp"

namespace {

AppState stateWith(bool simpleMode, bool opening, int hiddenDice) {
    AppState state;
    state.simpleMode = simpleMode;
    state.opening = opening;
    state.hiddenDice = hiddenDice;
    return state;
}

ImVec2 fittedSize(const AppState& state) {
    return measureLayout([drawnState = state]() mutable { return drawApp(drawnState); });
}

ImVec2 reportedSizeAt(AppState state, ImVec2 size) {
    ImGui::GetIO().DisplaySize = size;
    ImGui::NewFrame();
    const ImVec2 reported = drawApp(state);
    ImGui::EndFrame();
    return reported;
}

}  // namespace

using AppLayout = HeadlessImGuiTest;

TEST_F(AppLayout, FittedSizeIsAFixedPoint) {
    const AppState states[] = {windowSizingState(), AppState{}, stateWith(true, true, 0),
                               stateWith(false, false, 35), stateWith(false, true, 10)};
    for (const AppState& state : states) {
        SCOPED_TRACE(testing::Message() << "simple " << state.simpleMode << ", opening "
                                        << state.opening << ", hidden " << state.hiddenDice);
        const ImVec2 fitted = fittedSize(state);
        const ImVec2 reported = reportedSizeAt(state, fitted);
        EXPECT_FLOAT_EQ(std::ceil(reported.x), fitted.x);
        EXPECT_FLOAT_EQ(std::ceil(reported.y), fitted.y);
    }
}

TEST_F(AppLayout, SizingStateIsTheLargestAtTheDefaultDice) {
    const ImVec2 sizing = fittedSize(windowSizingState());
    for (const bool simpleMode : {true, false}) {
        for (const bool opening : {true, false}) {
            const ImVec2 fitted = fittedSize(stateWith(simpleMode, opening, 10));
            EXPECT_LE(fitted.x, sizing.x);
            EXPECT_LE(fitted.y, sizing.y);
        }
    }
}

TEST_F(AppLayout, FittedSizeGrowsWithTheDisplayScale) {
    const ImVec2 atOne = fittedSize(windowSizingState());
    DisplayScale(2.0f).applyTo(ImGui::GetStyle());
    const ImVec2 atTwo = fittedSize(windowSizingState());
    EXPECT_GT(atTwo.x, 1.8f * atOne.x);
    EXPECT_GT(atTwo.y, 1.8f * atOne.y);
}
