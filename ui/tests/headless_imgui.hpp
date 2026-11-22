#pragma once

#include <gtest/gtest.h>

#include <imgui.h>

class HeadlessImGuiTest : public ::testing::Test {
protected:
    void SetUp() override {
        ImGui::CreateContext();
        ImGuiIO& inputOutput = ImGui::GetIO();
        inputOutput.IniFilename = nullptr;
        inputOutput.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
        inputOutput.Fonts->AddFontDefault();
    }

    void TearDown() override { ImGui::DestroyContext(); }
};
