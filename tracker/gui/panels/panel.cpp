#include <utility>

#include "../../general.hpp"
#include "../../song/lock/registry.hpp"
#include "../init.hpp"
#include "../keys.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../history/actions/action.hpp"
#include "../themes/theme.hpp"
#include "panel.hpp"

GUIPanel::GUIPanel(
    std::string label,
    bool visible,
    bool windowed
)
    : label(std::move(label)),
      visible(visible),
      windowed(windowed) {
}

void GUIPanel::lock_item(const Target target, const size_t index) {
    ImGui::SameLine();
    const bool is_locked = lock_registry.is_locked(target, index);
    if (is_locked) {
        ImGui::PushStyleColor(ImGuiCol_Button, theme.get_u32_color(ThemeItem::LockButton));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme.get_u32_color(ThemeItem::LockButtonHover));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme.get_u32_color(ThemeItem::LockButtonActive));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, theme.get_u32_color(ThemeItem::UnlockButton));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme.get_u32_color(ThemeItem::UnlockButtonHover));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme.get_u32_color(ThemeItem::UnlockButtonActive));
    }
    if (ImGui::Button("L")) {
        if (index != -1) {
            const bool locked = lock_registry.toggle_lock(target, index);
            const LinkKey key = {target, static_cast<int>(index), 0};
            perform_action_lock(this, key, locked);
        }
    }
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(is_locked ? "Unlock" : "Lock");
    }
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

    if (ImGui::BeginPopupModal("Confirm item removal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove this item?\nThis action may be only partially restored.\n");

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
    if (windowed) {
        ImGui::Begin(label.c_str());
    }

    ImGui::BeginDisabled(is_disabled());

    focus = false;
    save = false;
    if (select_item()) {
        from();
        pre_actions();
        if (visible) {
            focus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            draw();
        }
        shortcut_actions();
        check_keyboard_input();
        draw_dialog_box();
        to();
        history_actions();
        post_actions();
    } else {
        empty();
        history_actions();
    }

    ImGui::EndDisabled();

    if (windowed) {
        ImGui::End();
    }
}

void GUIPanel::add_action(Action *action, const bool undo) {
    pending_actions.emplace_back(action, undo);
}

bool GUIPanel::is_focused() const {
    return focus;
}

void GUIPanel::set_focus(const bool focus) {
    this->focus = focus;
    if (focus) {
        ImGui::SetWindowFocus(label.c_str());
    }
}

void GUIPanel::initialize() {
    from();
    update();
    register_shortcuts();
}

void GUIPanel::history_actions() {
    if (pending_actions.empty()) {
        return;
    }

    save = true;
    for (const auto &[action, undo] : pending_actions) {
        const int index = action->key.index;
        set_index(index);
        from();
        if (undo) {
            action->undo();
        } else {
            action->redo();
        }
        to();
    }
    pending_actions.clear();
}
