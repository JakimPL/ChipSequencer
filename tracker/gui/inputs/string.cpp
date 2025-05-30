#include "string.hpp"

StringInputHandler::StringInputHandler(const std::vector<ImGuiKey> keys, const bool synchronize)
    : keys(keys), synchronize(synchronize) {
}

bool StringInputHandler::handle_input(std::vector<std::string> &strings, int &index) {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return false;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return false;
    }

    bool value_inserted = false;
    if (capture_input || synchronize || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        buffer = strings[index];
        capture_input = false;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (!buffer.empty()) {
            buffer.pop_back();
        }
    } else if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        buffer.clear();
    }

    for (const int key : keys) {
        if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {
            char input_character = 0;
            if (key >= ImGuiKey_A && key <= ImGuiKey_Z) {
                input_character = 'A' + (key - ImGuiKey_A);
            } else if (key >= ImGuiKey_0 && key <= ImGuiKey_9) {
                input_character = '0' + (key - ImGuiKey_0);
            } else if (key == ImGuiKey_Period) {
                input_character = '.';
            } else if (key == ImGuiKey_Comma) {
                input_character = ',';
            }

            if (buffer.size() >= limit) {
                buffer.pop_back();
            }

            buffer.push_back(input_character);
            value_inserted = true;
        }
    }

    try {
        strings[index] = buffer;
    } catch (std::out_of_range &) {
        buffer.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        index = std::max(0, index - 1);
        buffer = strings[index];
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        index = std::min(static_cast<int>(strings.size()) - 1, index + 1);
        buffer = strings[index];
    }

    return value_inserted;
}
