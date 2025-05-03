#include "input.hpp"

InputHandler::InputHandler(std::vector<int> &vector, int &index)
    : vector(vector), index(index) {
}

void InputHandler::handle_input() {
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

    if (numeric) {
        for (int key = ImGuiKey_0; key <= ImGuiKey_9; key++) {
            if (ImGui::IsKeyPressed((ImGuiKey) key)) {
                buffer.push_back('0' + (key - ImGuiKey_0));
            }
        }

        if (!buffer.empty()) {
            try {
                int value = std::stoi(buffer);
                value = std::max(std::min(value, limit), 0);
                vector[index] = value;
            } catch (std::out_of_range &) {
                buffer.clear();
            }
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        buffer = std::to_string(vector[index]);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        index = std::max(0, index - 1);
        buffer.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        index = std::min(static_cast<int>(vector.size()) - 1, index + 1);
        buffer.clear();
    }
}

void InputHandler::set_limit(const int max) {
    limit = max;
}

void InputHandler::clear() {
    buffer.clear();
}
