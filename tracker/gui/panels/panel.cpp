#include "panel.hpp"

GUIPanel::GUIPanel(const bool visible)
    : visible(visible) {
}

void GUIPanel::draw_add_or_remove() {
    if (ImGui::Button("-")) {
        ImGui::OpenPopup("Confirm item removal");
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) {
        add();
    }
    ImGui::SameLine();

    if (ImGui::BeginPopupModal("Confirm item removal", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove this item?\nThis action cannot be undone.\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            remove();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void GUIPanel::frame() {
    if (visible) {
        draw();
    }
}
