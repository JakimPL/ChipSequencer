#include "../../general.hpp"
#include "numeric.hpp"

bool NumericInputHandler::handle_input(std::vector<int> &vector, int &index) {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return false;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return false;
    }

    if (capture_input || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        buffer = std::to_string(vector[index]);
        capture_input = false;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (!buffer.empty()) {
            buffer.pop_back();
        }
    }

    bool value_inserted = false;
    for (int key = ImGuiKey_0; key <= ImGuiKey_9; key++) {
        if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {
            buffer.push_back('0' + (key - ImGuiKey_0));
        }

        value_inserted = true;
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditIncrement)) {
        buffer = std::to_string(vector[index]);
        assign_value(vector, index, 1);
    } else if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditDecrement)) {
        buffer = std::to_string(vector[index]);
        assign_value(vector, index, -1);
    } else {
        assign_value(vector, index);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        index = std::max(0, index - 1);
        buffer.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        index = std::min(static_cast<int>(vector.size()) - 1, index + 1);
        buffer.clear();
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditHome)) {
        index = 0;
        buffer.clear();
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditEnd)) {
        index = static_cast<int>(vector.size()) - 1;
        buffer.clear();
    }

    return value_inserted;
}

void NumericInputHandler::assign_value(std::vector<int> &vector, const size_t index, const int adjustment) {
    if (buffer.empty()) {
        return;
    }

    try {
        int value = std::stoi(buffer);
        value = std::max(std::min(value + adjustment, limit), 0);
        vector[index] = value;
        buffer = std::to_string(value);
    } catch (std::out_of_range &) {
        this->buffer.clear();
    }
}
