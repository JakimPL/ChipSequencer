#include "../../general.hpp"
#include "../../tuning/frequencies.hpp"
#include "../../utils/string.hpp"
#include "../constants.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "../clipboard/clipboard.hpp"
#include "../history/manager.hpp"
#include "../shortcuts/manager.hpp"
#include "../themes/theme.hpp"
#include "editor.hpp"

GUIEditorPanel::GUIEditorPanel(const bool visible, const bool windowed)
    : GUIPanel("Editor", visible, windowed) {
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
        if (ImGui::BeginTabItem("Clipboard")) {
            draw_clipboard();
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
        ImGui::TextColored(theme.get_vec4_color(ThemeItem::TextUnavailable), "No actions recorded yet.");
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
            ImGui::PushStyleColor(ImGuiCol_Text, theme.get_vec4_color(ThemeItem::HistoryCurrentAction));
        } else if (!is_applied) {
            ImGui::PushStyleColor(ImGuiCol_Text, theme.get_vec4_color(ThemeItem::HistoryUnappliedAction));
        }

        const std::string action_label = action_name + "##" + std::to_string(i + 1);
        char label[GUI_MAX_HISTORY_ITEM_LENGTH];
        copy_string_to_buffer(action_label, label, sizeof(label));

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

void GUIEditorPanel::draw_clipboard() {
    auto get_category_name = [](ClipboardCategory category) -> const char * {
        switch (category) {
        case ClipboardCategory::Notes:
            return "Notes";
        case ClipboardCategory::Commands:
            return "Commands";
        case ClipboardCategory::None:
        default:
            return "Unknown";
        }
    };

    ImGui::BeginChild("ClipboardPanel", ImVec2(0, 0), 0);
    ImGui::BeginDisabled(gui.is_playing());
    ImGui::Separator();

    bool has_any_items = false;

    for (auto category : {ClipboardCategory::Notes, ClipboardCategory::Commands}) {
        const auto *items = clipboard.get_items(category);
        if (items != nullptr && !items->empty()) {
            has_any_items = true;
            break;
        }
    }

    if (!has_any_items) {
        ImGui::TextColored(theme.get_vec4_color(ThemeItem::TextUnavailable), "No clipboard items yet.");
        ImGui::EndDisabled();
        ImGui::EndChild();
        return;
    }

    const float total_width = ImGui::GetContentRegionAvail().x;
    ImGui::Columns(3, "ClipboardColumns", false);
    ImGui::SetColumnWidth(0, total_width * 0.2f);
    ImGui::SetColumnWidth(1, total_width * 0.6f);

    ImGui::Text("Category");
    ImGui::NextColumn();
    ImGui::Text("Item");
    ImGui::NextColumn();
    ImGui::Text("Position");
    ImGui::NextColumn();
    ImGui::Separator();

    for (auto category : {ClipboardCategory::Notes, ClipboardCategory::Commands}) {
        const auto *items = clipboard.get_items(category);
        if (items == nullptr || items->empty()) {
            continue;
        }

        const char *category_name = get_category_name(category);

        for (size_t i = 0; i < items->size(); ++i) {
            const auto &item = (*items)[i];

            if (i == 0) {
                ImGui::Text("%s", category_name);
            }

            ImGui::NextColumn();

            const std::string item_label = item->get_name() + "##" + std::to_string(static_cast<int>(category)) + "_" + std::to_string(i);
            char label[GUI_MAX_HISTORY_ITEM_LENGTH];
            copy_string_to_buffer(item_label, label, sizeof(label));

            if (i == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, theme.get_vec4_color(ThemeItem::ClipboardRecent));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, theme.get_vec4_color(ThemeItem::ClipboardOlder));
            }

            if (ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns)) {
                clipboard.move_item_to_front(category, i);
            }

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Click to move to front (making it the active clipboard item)");
                ImGui::EndTooltip();
            }

            ImGui::PopStyleColor();
            ImGui::NextColumn();

            if (i == 0) {
                ImGui::Text("Most recent");
            } else {
                ImGui::Text("%zu", i + 1);
            }
            ImGui::NextColumn();
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
