
#include "../names.hpp"
#include "../actions/action.hpp"
#include "panel.hpp"

GUIPanel::GUIPanel(const bool visible)
    : visible(visible) {
}

void GUIPanel::draw_add_or_remove(
    const std::vector<std::string> &dependencies,
    const std::vector<std::pair<ItemType, uint8_t>> &link_dependencies
) {
    if (ImGui::Button("-")) {
        if (dependencies.empty() && link_dependencies.empty()) {
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
        ImGui::Text("Are you sure you want to remove this item?\nThis action cannot be undone.\n");

        if (!dependencies.empty()) {
            ImGui::Text("\nThis item is used by the following items:");
            for (size_t i = 0; i < dependencies.size(); ++i) {
                if (i < dependencies.size() - 1) {
                    const std::string name = dependencies[i] + ",";
                    ImGui::Text("%s", name.c_str());
                    ImGui::SameLine();
                } else {
                    ImGui::Text("%s", dependencies[i].c_str());
                }
            }
        }

        if (!link_dependencies.empty()) {
            ImGui::Text("\nThis item is linked to the following items:");
            for (const auto &link : link_dependencies) {
                const std::string &name = item_types_names.at(link.first);
                ImGui::Text("%s %d", name.c_str(), link.second);
                if (&link != &link_dependencies.back()) {
                    ImGui::SameLine();
                }
            }
            ImGui::Text("\nAll links are going to be destroyed.");
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

void GUIPanel::add_action(Action *value_change, const bool undo) {
    if (value_change != nullptr) {
        pending_actions.emplace_back(value_change, undo);
    }
}

void GUIPanel::actions() {
    for (const auto &[value_change, undo] : pending_actions) {
        if (undo) {
            value_change->undo();
        } else {
            value_change->redo();
        }
    }
}

void GUIPanel::post_actions() {
    if (!pending_actions.empty()) {
        throw std::runtime_error("Pending actions not executed: " + pending_actions.front().first->name);
    }
    pending_actions.clear();
}
