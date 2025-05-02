#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "commands.hpp"

GUICommandsPanel::GUICommandsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUICommandsPanel::draw() {
    ImGui::Begin("Commands");
    ImGui::Columns(1, "commands_columns");

    std::vector<size_t> dependencies = song.find_commands_dependencies(commands_index);
    push_tertiary_style();
    draw_add_or_remove("orders", dependencies);
    prepare_combo(commands_names, "##SequenceCombo", commands_index);
    show_dependency_tooltip("orders", dependencies);
    pop_tertiary_style();

    ImGui::Separator();

    from();
    draw_sequence();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUICommandsPanel::is_index_valid() const {
    // return commands_index >= 0 && commands_index < commands.size();
    return true;
}

void GUICommandsPanel::from() {
    if (!is_index_valid()) {
        return;
    }
}

void GUICommandsPanel::to() const {
    if (!ImGui::IsWindowFocused() || !is_index_valid()) {
        return;
    }
}

void GUICommandsPanel::add() {
    // Commands *new_commands = song.add_commands();
    // if (new_commands == nullptr) {
    //     return;
    // }

    // commands_index = commands.size() - 1;
    // update();
}

void GUICommandsPanel::duplicate() {
    // Commands *new_commands = song.duplicate_commands(commands_index);
    // if (new_commands == nullptr) {
    //     return;
    // }

    // commands_index = commands.size() - 1;
    // update();
}

void GUICommandsPanel::remove() {
    if (is_index_valid()) {
        // song.remove_commands(commands_index);
        // commands_index = std::max(0, commands_index - 1);
        // update();
    }
}

void GUICommandsPanel::update() {
    // update_items(commands_names, commands.size(), "Commands ", commands_index);
    // gui.update(GUIElement::Orders);
}

void GUICommandsPanel::draw_sequence() {
}

void GUICommandsPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
}

void GUICommandsPanel::set_index(const int index) {
    // commands_index = clamp_index(index, commands.size());
}
