#include "app.hpp"
#include "display_fit.hpp"
#include "embedded_droid_sans.hpp"
#include "theme.hpp"

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

namespace {

void glfwErrorCallback(int error, const char* description) {
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

constexpr float kFontSize = 14.0f;

void loadFont() {
    ImFontAtlas* fonts = ImGui::GetIO().Fonts;
    ImFontConfig fontConfig;
    fontConfig.Flags |= ImFontFlags_NoLoadError;
    // The font lives in static storage: ImGui must not free it.
    fontConfig.FontDataOwnedByAtlas = false;
    if (fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(liars_dice::assets::kDroidSansTtf),
                                    static_cast<int>(liars_dice::assets::kDroidSansTtfSize), kFontSize,
                                    &fontConfig) == nullptr) {
        std::fprintf(stderr, "Failed to load embedded font, using default\n");
        fonts->AddFontDefault();
    }
}

}  // namespace

int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        return 1;
    }

    const char* glslVersion = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Liar's Dice", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;

    applyLiarsDiceTheme();

    loadFont();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    FittedWindow fittedWindow(
        window, ImGui::GetStyle(),
        [sizingState = windowSizingState()]() mutable { return drawApp(sizingState); });

    AppState state;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        fittedWindow.beginFrame();
        ImGui::NewFrame();

        drawApp(state);

        ImGui::Render();
        int displayWidth = 0;
        int displayHeight = 0;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
