#include "theme.hpp"

#include "format.hpp"

namespace {

ImVec4 slate(int red, int green, int blue, int alpha = 255) {
    return ImVec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
}

}  // namespace

ImU32 legalRaiseBackground(double chanceTrue) {
    const LegalRaiseColor color = legalRaiseColor(chanceTrue);
    return IM_COL32(color.red, color.green, color.blue, legalRaiseAlpha(chanceTrue));
}

void applyLiarsDiceTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = slate(236, 231, 223);
    colors[ImGuiCol_TextDisabled] = slate(122, 128, 134);
    colors[ImGuiCol_WindowBg] = slate(27, 32, 36);
    colors[ImGuiCol_ChildBg] = slate(27, 32, 36, 0);
    colors[ImGuiCol_PopupBg] = slate(20, 24, 27);
    colors[ImGuiCol_Border] = slate(45, 80, 78);
    colors[ImGuiCol_BorderShadow] = slate(0, 0, 0, 0);
    colors[ImGuiCol_FrameBg] = slate(36, 43, 48);
    colors[ImGuiCol_FrameBgHovered] = slate(45, 54, 60);
    colors[ImGuiCol_FrameBgActive] = slate(53, 64, 71);
    colors[ImGuiCol_TitleBg] = slate(20, 24, 27);
    colors[ImGuiCol_TitleBgActive] = slate(27, 32, 36);
    colors[ImGuiCol_TitleBgCollapsed] = slate(20, 24, 27);
    colors[ImGuiCol_MenuBarBg] = slate(20, 24, 27);
    colors[ImGuiCol_ScrollbarBg] = slate(20, 24, 27);
    colors[ImGuiCol_ScrollbarGrab] = slate(53, 64, 71);
    colors[ImGuiCol_ScrollbarGrabHovered] = slate(62, 138, 128);
    colors[ImGuiCol_ScrollbarGrabActive] = slate(77, 191, 175);
    colors[ImGuiCol_CheckMark] = slate(77, 191, 175);
    colors[ImGuiCol_SliderGrab] = slate(62, 138, 128);
    colors[ImGuiCol_SliderGrabActive] = slate(77, 191, 175);
    colors[ImGuiCol_Button] = slate(36, 43, 48);
    colors[ImGuiCol_ButtonHovered] = slate(45, 54, 60);
    colors[ImGuiCol_ButtonActive] = slate(53, 96, 90);
    colors[ImGuiCol_Header] = slate(45, 54, 60);
    colors[ImGuiCol_HeaderHovered] = slate(53, 64, 71);
    colors[ImGuiCol_HeaderActive] = slate(53, 96, 90);
    colors[ImGuiCol_Separator] = slate(45, 80, 78);
    colors[ImGuiCol_SeparatorHovered] = slate(62, 138, 128);
    colors[ImGuiCol_SeparatorActive] = slate(77, 191, 175);
    colors[ImGuiCol_TableHeaderBg] = slate(32, 38, 42);
    colors[ImGuiCol_TableBorderStrong] = slate(53, 64, 71);
    colors[ImGuiCol_TableBorderLight] = slate(40, 47, 52);
    colors[ImGuiCol_TableRowBg] = slate(0, 0, 0, 0);
    colors[ImGuiCol_TableRowBgAlt] = slate(31, 37, 41, 40);
    colors[ImGuiCol_TextLink] = slate(77, 191, 175);
    colors[ImGuiCol_TextSelectedBg] = slate(62, 138, 128, 120);

    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
}
