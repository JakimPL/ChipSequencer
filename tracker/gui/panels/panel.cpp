#include "panel.hpp"

GUIPanel::GUIPanel(const bool visible)
    : visible(visible) {
}

void GUIPanel::draw_add_or_remove(const std::string label, std::vector<size_t> dependencies) {
    if (ImGui::Button("-")) {
        if (dependencies.empty()) {
            remove();
        } else {
            ImGui::OpenPopup("Confirm item removal");
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove");
    }
    ImGui::SameLine();
    if (ImGui::Button("D")) {
        duplicate();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Duplicate");
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) {
        add();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Add");
    }
    ImGui::SameLine();

    if (ImGui::BeginPopupModal("Confirm item removal", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove this item?\nThis action cannot be undone.\n\n");
        ImGui::Text("This item is used by the following %s:", label.c_str());
        for (size_t i = 0; i < dependencies.size(); ++i) {
            ImGui::Text("%zu", dependencies[i]);
            if (i < dependencies.size() - 1) {
                ImGui::SameLine();
            }
        }
        ImGui::Separator();

        const float button_width = 120.0f;
        const float total_button_width = (button_width * 2) + ImGui::GetStyle().ItemSpacing.x;
        const float available_width = ImGui::GetContentRegionAvail().x;
        const float offset_x = (available_width - total_button_width) * 0.5f;

        if (offset_x > 0.0f) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
        }

        if (ImGui::Button("OK", ImVec2(button_width, 0))) {
            remove();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(button_width, 0))) {
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
