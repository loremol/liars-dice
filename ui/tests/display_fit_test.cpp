#include <gtest/gtest.h>

#include <imgui.h>

#include "display_fit.hpp"
#include "headless_imgui.hpp"

TEST(DisplayScale, ScalesSpacingAndFonts) {
    ImGuiStyle style;
    DisplayScale(1.5f).applyTo(style);
    EXPECT_FLOAT_EQ(style.WindowPadding.x, 12.0f);
    EXPECT_FLOAT_EQ(style.FontScaleDpi, 1.5f);
}

TEST(DisplayScale, OneLeavesTheStyleAsItIs) {
    ImGuiStyle style;
    DisplayScale(1.0f).applyTo(style);
    EXPECT_FLOAT_EQ(style.WindowPadding.x, 8.0f);
    EXPECT_FLOAT_EQ(style.FontScaleDpi, 1.0f);
}

TEST(DisplayScale, ANonPositiveFactorFallsBackToOne) {
    EXPECT_FLOAT_EQ(DisplayScale(0.0f).factor(), 1.0f);
    EXPECT_FLOAT_EQ(DisplayScale(-2.0f).factor(), 1.0f);
}

TEST(ScreenSize, IsThePixelSizeOnX11) {
    const ImVec2 size = screenSize(ImVec2(574.0f, 406.0f), 1.0f);
    EXPECT_FLOAT_EQ(size.x, 574.0f);
    EXPECT_FLOAT_EQ(size.y, 406.0f);
}

TEST(ScreenSize, RoundsUpUnderFractionalScaling) {
    const ImVec2 size = screenSize(ImVec2(861.0f, 610.0f), 1.5f);
    EXPECT_FLOAT_EQ(size.x, 574.0f);
    EXPECT_FLOAT_EQ(size.y, 407.0f);
}

using MeasureLayout = HeadlessImGuiTest;

TEST_F(MeasureLayout, ReturnsTheSizeOnceTwoFramesInARowAgree) {
    int frames = 0;
    const ImVec2 size = measureLayout([&frames] {
        ++frames;
        return frames == 1 ? ImVec2(100.0f, 50.0f) : ImVec2(320.4f, 240.0f);
    });
    EXPECT_EQ(frames, 3);
    EXPECT_FLOAT_EQ(size.x, 321.0f);
    EXPECT_FLOAT_EQ(size.y, 240.0f);
}

TEST_F(MeasureLayout, GivesUpOnALayoutThatNeverSettles) {
    int frames = 0;
    const ImVec2 size = measureLayout([&frames] {
        ++frames;
        return ImVec2(100.0f + frames, 100.0f);
    });
    EXPECT_EQ(frames, 10);
    EXPECT_FLOAT_EQ(size.x, 110.0f);
}

TEST_F(MeasureLayout, DrawsOnADisplayLargerThanAnyScreen) {
    ImVec2 displaySize;
    measureLayout([&displaySize] {
        displaySize = ImGui::GetIO().DisplaySize;
        return ImVec2(1.0f, 1.0f);
    });
    EXPECT_GE(displaySize.x, 100000.0f);
    EXPECT_GE(displaySize.y, 100000.0f);
}

TEST_F(MeasureLayout, LeavesNoFrameOpen) {
    measureLayout([] { return ImVec2(1.0f, 1.0f); });
    ImGui::GetIO().DisplaySize = ImVec2(800.0f, 600.0f);
    ImGui::NewFrame();
    ImGui::EndFrame();
}
