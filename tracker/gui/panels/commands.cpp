#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "commands.hpp"

GUICommandsSequencesPanel::GUICommandsSequencesPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUICommandsSequencesPanel::draw() {
    ImGui::Begin("Commands");
    ImGui::Columns(1, "commands_columns");

    std::vector<size_t> dependencies = song.find_commands_dependencies(sequence_index);
    push_tertiary_style();
    draw_add_or_remove("orders", dependencies);
    prepare_combo(commands_names, "##CommandsCombo", sequence_index);
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

bool GUICommandsSequencesPanel::is_index_valid() const {
    return sequence_index >= 0 && sequence_index < commands_sequences.size();
}

void GUICommandsSequencesPanel::from() {
    if (!is_index_valid()) {
        return;
    }
}

void GUICommandsSequencesPanel::to() const {
    if (!ImGui::IsWindowFocused() || !is_index_valid()) {
        return;
    }
}

void GUICommandsSequencesPanel::add() {
    CommandsSequence *new_sequence = song.add_commands_sequence();
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = commands_sequences.size() - 1;
    update();
}

void GUICommandsSequencesPanel::duplicate() {
    CommandsSequence *new_sequence = song.duplicate_commands_sequence(sequence_index);
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = commands_sequences.size() - 1;
    update();
}

void GUICommandsSequencesPanel::remove() {
    if (is_index_valid()) {
        song.remove_commands_sequence(sequence_index);
        sequence_index = std::max(0, sequence_index - 1);
        update();
    }
}

void GUICommandsSequencesPanel::update() {
    update_items(commands_names, commands_sequences.size(), "Commands ", sequence_index);
    gui.update(GUIElement::Orders);
}

void GUICommandsSequencesPanel::draw_sequence() {
}

void GUICommandsSequencesPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
}

void GUICommandsSequencesPanel::set_index(const int index) {
    sequence_index = clamp_index(index, commands_sequences.size());
}
