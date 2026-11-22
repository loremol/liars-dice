#include "display_fit.hpp"

#include <cmath>
#include <utility>

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>

namespace {

constexpr float kVirtualDisplaySide = 100000.0f;
constexpr int kMaxMeasuringFrames = 10;
constexpr double kWaylandScaleWaitSeconds = 0.1;

ImVec2 drawHiddenFrame(const DrawFrame& drawFrame) {
    ImGui::GetIO().DisplaySize = ImVec2(kVirtualDisplaySide, kVirtualDisplaySide);
    ImGui::NewFrame();
    const ImVec2 size = drawFrame();
    ImGui::EndFrame();
    return size;
}

float pixelsPerScreenCoordinate(GLFWwindow* window) {
    int windowWidth = 0;
    int framebufferWidth = 0;
    glfwGetWindowSize(window, &windowWidth, nullptr);
    glfwGetFramebufferSize(window, &framebufferWidth, nullptr);
    return windowWidth > 0 ? static_cast<float>(framebufferWidth) / windowWidth : 1.0f;
}

ImVec2 cursorInPixels(GLFWwindow* window, double cursorX, double cursorY) {
    const double pixels = pixelsPerScreenCoordinate(window);
    return ImVec2(static_cast<float>(cursorX * pixels), static_cast<float>(cursorY * pixels));
}

void forwardCursorInPixels(GLFWwindow* window, double cursorX, double cursorY) {
    const ImVec2 cursor = cursorInPixels(window, cursorX, cursorY);
    ImGui_ImplGlfw_CursorPosCallback(window, cursor.x, cursor.y);
}

}  // namespace

DisplayScale::DisplayScale(float factor) : factor_(factor > 0.0f ? factor : 1.0f) {}

DisplayScale DisplayScale::ofWindow(GLFWwindow* window) {
    float factor = 1.0f;
    glfwGetWindowContentScale(window, &factor, nullptr);
    return DisplayScale(factor);
}

float DisplayScale::factor() const { return factor_; }

void DisplayScale::applyTo(ImGuiStyle& style) const {
    style.ScaleAllSizes(factor_);
    style.FontScaleDpi = factor_;
}

ImVec2 measureLayout(const DrawFrame& drawFrame) {
    ImVec2 size = drawHiddenFrame(drawFrame);
    for (int frame = 1; frame < kMaxMeasuringFrames; ++frame) {
        const ImVec2 previous = size;
        size = drawHiddenFrame(drawFrame);
        if (size.x == previous.x && size.y == previous.y) {
            break;
        }
    }
    return ImVec2(std::ceil(size.x), std::ceil(size.y));
}

ImVec2 screenSize(ImVec2 pixels, float pixelsPerScreenCoordinate) {
    return ImVec2(std::ceil(pixels.x / pixelsPerScreenCoordinate),
                  std::ceil(pixels.y / pixelsPerScreenCoordinate));
}

FittedWindow::FittedWindow(GLFWwindow* window, const ImGuiStyle& unscaledStyle,
                           DrawFrame drawSizingFrame)
    : window_(window),
      unscaledStyle_(unscaledStyle),
      drawSizingFrame_(std::move(drawSizingFrame)),
      scale_(DisplayScale::ofWindow(window)) {
    glfwSetCursorPosCallback(window, forwardCursorInPixels);
    fit(scale_);
    glfwShowWindow(window);
    if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
        const double deadline = glfwGetTime() + kWaylandScaleWaitSeconds;
        while (DisplayScale::ofWindow(window) == scale_ && glfwGetTime() < deadline) {
            glfwWaitEventsTimeout(deadline - glfwGetTime());
        }
    }
}

void FittedWindow::beginFrame() {
    ImGuiIO& inputOutput = ImGui::GetIO();
    inputOutput.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    if (const DisplayScale scale = DisplayScale::ofWindow(window_); scale != scale_) {
        fit(scale);
    }
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    inputOutput.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    if (glfwGetWindowAttrib(window_, GLFW_FOCUSED) && !glfwGetWindowAttrib(window_, GLFW_HOVERED)) {
        double cursorX = 0.0;
        double cursorY = 0.0;
        glfwGetCursorPos(window_, &cursorX, &cursorY);
        const ImVec2 cursor = cursorInPixels(window_, cursorX, cursorY);
        inputOutput.AddMousePosEvent(cursor.x, cursor.y);
    }
}

void FittedWindow::fit(DisplayScale scale) {
    scale_ = scale;
    ImGui::GetStyle() = unscaledStyle_;
    scale.applyTo(ImGui::GetStyle());
    const ImVec2 size =
        screenSize(measureLayout(drawSizingFrame_), pixelsPerScreenCoordinate(window_));
    const int width = static_cast<int>(size.x);
    const int height = static_cast<int>(size.y);
    glfwSetWindowSizeLimits(window_, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowSize(window_, width, height);
}
