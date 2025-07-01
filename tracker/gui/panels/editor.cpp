#include "../../general.hpp"
#include "../../tuning/frequencies.hpp"
#include "../../utils/string.hpp"
#include "../constants.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../history/manager.hpp"
#include "../history/shortcuts.hpp"
#include "editor.hpp"

GUIEditorPanel::GUIEditorPanel(const bool visible, const bool windowed)
    : GUIPanel("Editor", visible, windowed) {
    initialize();
}

GUIElement GUIEditorPanel::get_element() const {
    return GUIElement::Editor;
}

void GUIEditorPanel::draw() {
    draw_tabs();
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
    if (ImGui::BeginTabBar("Options")) {
        if (ImGui::BeginTabItem("General")) {
            const int min_octave = frequency_table.get_min_octave();
            const int max_octave = frequency_table.get_max_octave();
            draw_int_slider(this, "Current octave", gui.current_octave, {}, min_octave, max_octave);
            draw_int_slider(this, "Jump step", gui.jump_step, {}, 0, GUI_MAX_JUMP_STEP);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Patterns")) {
            ImGui::Checkbox("Follow playback", &gui.follow_playback);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", shortcut_manager.get_shortcut_display(ShortcutAction::PlayerFollowPlayback).c_str());
            }

            ImGui::Checkbox("Repeat patterns", &gui.repeat_patterns);
            ImGui::Combo("Row display style", &gui.row_display_index, row_display_style_names.data(), row_display_style_names.size());
            gui.row_display = static_cast<RowDisplayStyle>(gui.row_display_index);

            const int previous_page_size = gui.page_size;
            draw_int_slider(this, "Page size", gui.page_size, {}, GUI_MIN_PAGE_SIZE, GUI_MAX_PAGE_SIZE);
            if (gui.page_size != previous_page_size) {
                gui.deselect_all_rows();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
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
        gui.current_octave = std::min(gui.current_octave + 1, max_octave);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadDivide)) {
        const int min_octave = frequency_table.get_min_octave();
        gui.current_octave = std::max(gui.current_octave - 1, min_octave);
    }
}
