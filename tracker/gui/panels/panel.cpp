#include "panel.hpp"

void GUIPanel::draw_add_or_remove() {
    if (ImGui::Button("+")) {
        add();
    }
    ImGui::SameLine();
    if (ImGui::Button("-")) {
        remove();
    }
    ImGui::SameLine();
}
