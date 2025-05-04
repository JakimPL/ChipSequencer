#include "string.hpp"

StringInputHandler::StringInputHandler(std::vector<std::string> &strings, int &index, const std::vector<ImGuiKey> keys)
    : strings(strings), InputHandler(index), keys(keys) {
}

void StringInputHandler::handle_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (!buffer.empty()) {
            buffer.pop_back();
        }
    }

    for (const int key : keys) {
        if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {
            char input_character = 0;
            if (key >= ImGuiKey_A && key <= ImGuiKey_Z) {
                input_character = 'A' + (key - ImGuiKey_A);
            } else if (key >= ImGuiKey_0 && key <= ImGuiKey_9) {
                input_character = '0' + (key - ImGuiKey_0);
            }

            if (buffer.size() >= limit) {
                buffer.pop_back();
            }
            buffer.push_back(input_character);
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Space) || !buffer.empty()) {
        try {
            strings[index] = buffer;
        } catch (std::out_of_range &) {
            buffer.clear();
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        buffer = strings[index];
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        index = std::max(0, index - 1);
        buffer.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        index = std::min(static_cast<int>(strings.size()) - 1, index + 1);
        buffer.clear();
    }
}
