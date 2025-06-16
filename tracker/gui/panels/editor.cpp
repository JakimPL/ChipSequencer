#include "../../general.hpp"
#include "../../utils/string.hpp"
#include "../constants.hpp"
#include "editor.hpp"

GUIEditorPanel::GUIEditorPanel(const bool visible, int &octave, int &step, int &page)
    : GUIPanel(visible), current_octave(octave), jump_step(step), page_size(page) {
}

void GUIEditorPanel::draw() {
    ImGui::Begin("Editor");

    if (select_item()) {
        check_keyboard_input();
        draw_tabs();
    } else {
        empty();
    }

    ImGui::End();
}

void GUIEditorPanel::draw_tabs() {
    if (ImGui::BeginTabBar("EditorTabs")) {
        if (ImGui::BeginTabItem("Options")) {
            draw_options();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("History")) {
            draw_history();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void GUIEditorPanel::draw_options() {
    const int min_octave = frequency_table.get_min_octave();
    const int max_octave = frequency_table.get_max_octave();
    ImGui::Text("Current octave:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##CurrentOctave", &current_octave, min_octave, max_octave);

    ImGui::Text("Jump step:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##JumpStep", &jump_step, 0, GUI_MAX_JUMP_STEP);

    const int previous_page_size = page_size;
    ImGui::Text("Page size:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##PageSize", &page_size, GUI_MIN_PAGE_SIZE, GUI_MAX_PAGE_SIZE);
    if (page_size != previous_page_size) {
        gui.deselect_all_rows();
    }
}

void GUIEditorPanel::draw_history() {
    const size_t current_index = history_manager.get_current_index();
    const size_t history_size = history_manager.get_history_size();

    ImGui::BeginChild("HistoryPanel", ImVec2(0, 0), false);
    ImGui::BeginDisabled(gui.is_playing());
    ImGui::Separator();

    if (ImGui::Button("Undo") && history_manager.can_undo()) {
        history_manager.undo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Redo") && history_manager.can_redo()) {
        history_manager.redo();
    }
    ImGui::Separator();

    if (history_size == 0) {
        ImGui::TextColored(GUI_TEXT_COLOR_UNAVAILABLE, "No actions recorded yet.");
        ImGui::EndDisabled();
        ImGui::EndChild();
        return;
    }

    const float total_width = ImGui::GetContentRegionAvail().x;
    ImGui::Columns(2, "HistoryColumns", false);
    ImGui::SetColumnWidth(0, total_width * 0.8f);

    ImGui::Text("Action");
    ImGui::NextColumn();
    ImGui::Text("Status");
    ImGui::NextColumn();
    ImGui::Separator();

    for (int i = history_size - 1; i >= 0; i--) {
        bool is_applied = (i < static_cast<int>(current_index));
        std::string action_name = history_manager.get_action_name(i);

        if (i == static_cast<int>(current_index) - 1) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
        } else if (!is_applied) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        }

        const std::string action_label = action_name + "##" + std::to_string(i + 1);
        char label[GUI_MAX_HISTORY_ITEM_LENGTH];
        copy_string_to_buffer(action_label.c_str(), label, sizeof(label));

        if (ImGui::Selectable(label, i == static_cast<int>(current_index) - 1, ImGuiSelectableFlags_SpanAllColumns)) {
            history_manager.go_to_index(i + 1);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Click to revert to this state");
            ImGui::EndTooltip();
        }

        ImGui::NextColumn();

        if (i == static_cast<int>(current_index) - 1) {
            ImGui::Text("Current");
        } else if (is_applied) {
            ImGui::Text("Applied");
        } else {
            ImGui::Text("Undone");
        }
        ImGui::NextColumn();

        if (i == static_cast<int>(current_index) - 1 || !is_applied) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::Columns(1);
    ImGui::EndDisabled();
    ImGui::EndChild();
}

void GUIEditorPanel::check_keyboard_input() {
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadMultiply)) {
        const int max_octave = frequency_table.get_max_octave();
        current_octave = std::min(current_octave + 1, max_octave);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadDivide)) {
        const int min_octave = frequency_table.get_min_octave();
        current_octave = std::max(current_octave - 1, min_octave);
    }
}
