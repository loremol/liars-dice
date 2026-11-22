#pragma once

#include <functional>

#include <imgui.h>

struct GLFWwindow;

class DisplayScale {
public:
    explicit DisplayScale(float factor);

    static DisplayScale ofWindow(GLFWwindow* window);

    float factor() const;

    void applyTo(ImGuiStyle& style) const;

    bool operator==(const DisplayScale&) const = default;

private:
    float factor_;
};

using DrawFrame = std::function<ImVec2()>;

ImVec2 measureLayout(const DrawFrame& drawFrame);

ImVec2 screenSize(ImVec2 pixels, float pixelsPerScreenCoordinate);

class FittedWindow {
public:
    FittedWindow(GLFWwindow* window, const ImGuiStyle& unscaledStyle, DrawFrame drawSizingFrame);

    void beginFrame();

private:
    void fit(DisplayScale scale);

    GLFWwindow* window_;
    ImGuiStyle unscaledStyle_;
    DrawFrame drawSizingFrame_;
    DisplayScale scale_;
};
