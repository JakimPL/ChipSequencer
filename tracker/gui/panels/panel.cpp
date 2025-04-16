#include "panel.hpp"

GUIPanel::GUIPanel(const bool visible)
    : visible(visible) {
}

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

void GUIPanel::frame() {
    if (visible) {
        draw();
    }
}
