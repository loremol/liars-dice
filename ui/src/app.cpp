#include "app.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include <imgui.h>

#include <liars_dice/probability.hpp>

#include "format.hpp"
#include "theme.hpp"

namespace {

constexpr const char* kSimpleModeTooltip =
    "Off: enter your dice; a bid's odds depend on its face.\n"
    "On: no dice faces needed, only how many dice are hidden.\n"
    "Then every face has the same odds, so the table has one column.";

bool drawFacePicker(liars_dice::probability::Face& face) {
    bool changed = false;
    for (int value = 1; value <= 6; ++value) {
        ImGui::SameLine();
        if (ImGui::RadioButton(faceText(value), face == value)) {
            face = value;
            changed = true;
        }
    }
    return changed;
}

void drawYourDice(AppState& state) {
    ImGui::SeparatorText("Your dice");

    int count = static_cast<int>(state.enteredDice.size());
    if (ImGui::InputInt("count", &count)) {
        state.enteredDice.resize(static_cast<std::size_t>(std::clamp(count, 1, 5)),
                                 liars_dice::probability::Face{1});
    }

    for (std::size_t dieIndex = 0; dieIndex < state.enteredDice.size(); ++dieIndex) {
        ImGui::PushID(static_cast<int>(dieIndex));
        ImGui::AlignTextToFramePadding();
        ImGui::Text("die %d", static_cast<int>(dieIndex) + 1);
        drawFacePicker(state.enteredDice[dieIndex]);
        ImGui::PopID();
    }
}

void drawHiddenDice(AppState& state) {
    ImGui::SeparatorText("Hidden dice");
    if (ImGui::InputInt("count##hiddenDice", &state.hiddenDice)) {
        state.hiddenDice = std::clamp(state.hiddenDice, 0, 35);
    }
}

void drawPreviousBidInput(AppState& state) {
    ImGui::SeparatorText("Previous bid");

    ImGui::Checkbox("I'm opening the round", &state.opening);

    ImGui::BeginDisabled(state.opening);

    state.clampPreviousBid();
    int quantity = state.previousBid.quantity();
    if (ImGui::InputInt("quantity", &quantity, 1)) {
        state.previousBid = liars_dice::probability::Bid{std::max(quantity, 1), state.previousBid.face()};
        state.clampPreviousBid();
    }

    if (!state.simpleMode) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("face");
        ImGui::PushID("previousBidFace");
        liars_dice::probability::Face face = state.previousBid.face();
        if (drawFacePicker(face)) {
            state.previousBid = liars_dice::probability::Bid{state.previousBid.quantity(), face};
        }
        ImGui::PopID();
    }

    ImGui::EndDisabled();
}

void drawPreviousBidOdds(const AppState& state) {
    if (state.opening) return;

    ImGui::SeparatorText("Previous bid odds");
    const liars_dice::probability::BidOdds previousBidOdds =
        liars_dice::probability::odds(state.previousBid, state.knownDice(), state.hiddenDice);
    ImGui::Text("Exactly right: %s",
                formatPercent(previousBidOdds.exactly, 1.0 - previousBidOdds.exactly, 1).c_str());
    ImGui::SameLine();
    ImGui::Text("Overshoot: %s",
                formatPercent(previousBidOdds.fewer, previousBidOdds.atLeast, 1).c_str());
}

void drawSwatch(ImU32 color, bool filled) {
    const float swatchSize = ImGui::GetTextLineHeight();
    const ImVec2 topLeft = ImGui::GetCursorScreenPos();
    const ImVec2 bottomRight(topLeft.x + swatchSize, topLeft.y + swatchSize);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (filled) {
        drawList->AddRectFilled(topLeft, bottomRight, color);
    } else {
        drawList->AddRect(topLeft, bottomRight, color);
    }
    ImGui::Dummy(ImVec2(swatchSize, swatchSize));
    ImGui::SameLine();
}

void drawLegend(bool opening) {
    drawSwatch(legalRaiseBackground(1.0), true);
    ImGui::TextUnformatted("legal raise");
    if (opening) return;

    ImGui::SameLine();
    drawSwatch(ImGui::GetColorU32(ImGuiCol_TextDisabled), false);
    ImGui::TextDisabled("not allowed");

    drawSwatch(kPreviousBidBackground, true);
    ImGui::TextUnformatted("previous bid");
}

void drawOddsTable(const AppState& state) {
    ImGui::SeparatorText(state.simpleMode ? "Chance each bid is true (hidden dice only)"
                                          : "Chance each bid is true");

    const std::vector<liars_dice::probability::Face> faces =
        state.simpleMode ? std::vector<liars_dice::probability::Face>{state.previousBid.face()}
                         : std::vector<liars_dice::probability::Face>{1, 2, 3, 4, 5, 6};
    const std::vector<liars_dice::probability::Face> knownDice = state.knownDice();
    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY;
    const float legendLines = state.opening ? 1.0f : 2.0f;
    const float legendHeight = legendLines * ImGui::GetTextLineHeightWithSpacing();
    const float rowHeight = ImGui::GetTextLineHeight() + 2.0f * ImGui::GetStyle().CellPadding.y;
    const float contentHeight = (state.totalDice() + 1) * rowHeight;
    const float minimumHeight = 6.0f * rowHeight;
    const float tableHeight = std::min(
        contentHeight, std::max(ImGui::GetContentRegionAvail().y - legendHeight, minimumHeight));
    const ImVec2 outerSize(0.0f, tableHeight);

    const int columnCount = 1 + static_cast<int>(faces.size());
    if (ImGui::BeginTable("oddsTable", columnCount, tableFlags, outerSize)) {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("quantity", ImGuiTableColumnFlags_WidthFixed);
        for (const liars_dice::probability::Face face : faces) {
            ImGui::TableSetupColumn(state.simpleMode ? "any face" : facePluralText(face));
        }
        ImGui::TableHeadersRow();
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(1.0f, 0.0f));

        for (int quantity = 1; quantity <= state.totalDice(); ++quantity) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%d", quantity);

            ImGui::PushID(quantity);
            for (const liars_dice::probability::Face face : faces) {
                ImGui::TableNextColumn();
                ImGui::PushID(face.value());

                const liars_dice::probability::Bid bid{quantity, face};
                const liars_dice::probability::BidOdds bidOdds =
                    liars_dice::probability::odds(bid, knownDice, state.hiddenDice);
                const std::string text = formatPercent(bidOdds.atLeast, bidOdds.fewer, 0);

                const bool isPrevious = !state.opening && state.previousBid == bid;
                const bool isLegalRaise =
                    state.opening || liars_dice::probability::isRaise(bid, state.previousBid);

                if (isPrevious) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, kPreviousBidBackground);
                } else if (isLegalRaise) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                           legalRaiseBackground(bidOdds.atLeast));
                }

                const ImGuiSelectableFlags cellFlags = isPrevious || isLegalRaise
                    ? ImGuiSelectableFlags_None
                    : ImGuiSelectableFlags_Disabled;
                ImGui::Selectable(text.c_str(), false, cellFlags);
                ImGui::SetItemTooltip("%s", bidOddsTooltip(bid, bidOdds, state.simpleMode).c_str());
                ImGui::PopID();
            }
            ImGui::PopID();
        }

        ImGui::PopStyleVar();
        ImGui::EndTable();
    }

    drawLegend(state.opening);
}

}  // namespace

ImVec2 drawApp(AppState& state) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("Liar's Dice", nullptr, windowFlags);

    ImGui::BeginGroup();
    ImGui::BeginChild("inputs", ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
    ImGui::PushItemWidth(ImGui::GetFontSize() * 6.0f);
    ImGui::Checkbox("Simple mode", &state.simpleMode);
    ImGui::SetItemTooltip("%s", kSimpleModeTooltip);

    if (!state.simpleMode) {
        drawYourDice(state);
    }
    drawHiddenDice(state);
    drawPreviousBidInput(state);
    ImGui::PopItemWidth();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginGroup();
    drawPreviousBidOdds(state);
    drawOddsTable(state);
    ImGui::EndGroup();
    ImGui::EndGroup();

    const ImVec2 padding = ImGui::GetStyle().WindowPadding;
    const ImVec2 contentSize = ImGui::GetItemRectSize();
    ImGui::End();
    return ImVec2(contentSize.x + 2.0f * padding.x, contentSize.y + 2.0f * padding.y);
}

AppState windowSizingState() {
    AppState state;
    state.simpleMode = false;
    state.opening = false;
    return state;
}
