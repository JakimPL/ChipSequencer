#include "../../general.hpp"
#include "manager.hpp"

std::string Shortcut::get_display_string() const {
    std::string display_string;
    if (ctrl) {
        display_string += "Ctrl+";
    }
    if (shift) {
        display_string += "Shift+";
    }
    if (alt) {
        display_string += "Alt+";
    }

    display_string += ImGui::GetKeyName(key);
    return display_string;
}

void ShortcutManager::register_shortcut(
    const ShortcutAction id,
    const std::function<void()> &action
) {
    actions[id].push_back(action);
}

const Shortcut &ShortcutManager::get_shortcut(const ShortcutAction id) const {
    static const Shortcut empty_shortcut = {false, false, false, ImGuiKey_None};
    auto it = shortcuts.find(id);
    if (it != shortcuts.end()) {
        return it->second;
    }

    return empty_shortcut;
}

std::string ShortcutManager::get_shortcut_display(const ShortcutAction id) const {
    auto it = shortcuts.find(id);
    if (it != shortcuts.end()) {
        return it->second.get_display_string();
    }

    return "";
}

void ShortcutManager::process_shortcuts() const {
    for (const auto &[action, shortcut] : shortcuts) {
        if (is_shortcut_pressed(shortcut)) {
            execute_action(action);
        }
    }
}

void ShortcutManager::execute_action(const ShortcutAction id) const {
    auto it = actions.find(id);
    if (it != actions.end()) {
        for (const auto &action : it->second) {
            action();
        }
    }
}

bool ShortcutManager::is_shortcut_pressed(const Shortcut &shortcut) {
    return ImGui::IsKeyPressed(shortcut.key) &&
           ImGui::GetIO().KeyCtrl == shortcut.ctrl &&
           ImGui::GetIO().KeyShift == shortcut.shift &&
           ImGui::GetIO().KeyAlt == shortcut.alt;
};

bool ShortcutManager::is_shortcut_pressed(const ShortcutAction &id) const {
    auto it = shortcuts.find(id);
    if (it != shortcuts.end()) {
        return is_shortcut_pressed(it->second);
    }
    return false;
}
