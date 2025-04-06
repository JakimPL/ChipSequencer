#include <algorithm>

#include "../../general.hpp"
#include "../enums.hpp"
#include "../mapping.hpp"
#include "../utils.hpp"
#include "sequences.hpp"

GUISequencesPanel::GUISequencesPanel() {
    from();
    update();
}

void GUISequencesPanel::draw() {
    ImGui::Begin("Sequence Editor");
    ImGui::Columns(1, "sequence_columns");

    draw_add_or_remove();
    prepare_combo(sequence_names, "##SequenceCombo", sequence_index);
    ImGui::Separator();

    from();
    draw_sequence();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUISequencesPanel::is_index_valid() const {
    return sequence_index >= 0 && sequence_index < sequences.size();
}

void GUISequencesPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    current_sequence.pattern.from_sequence(sequence_index);
}

void GUISequencesPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Sequence *sequence = sequences[sequence_index];
    const std::vector<Note> note_vector = current_sequence.pattern.to_note_vector();
    Sequence *new_sequence = Pattern::to_sequence(note_vector);
    sequences[sequence_index] = new_sequence;
    delete sequence;
}

void GUISequencesPanel::add() {
    Sequence *new_sequence = song.add_sequence();
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = sequences.size() - 1;
    update();
}

void GUISequencesPanel::remove() {
    if (is_index_valid()) {
        song.remove_sequence(sequence_index);
        sequence_index = std::max(0, sequence_index - 1);
        update();
    }
}

void GUISequencesPanel::update() {
    update_items(sequence_names, sequences.size(), "Sequence ", sequence_index);
    gui.update(GUIElement::Orders);
}

void GUISequencesPanel::draw_sequence_length() {
    draw_number_of_items("Steps", "##SequenceLength", current_sequence.pattern.steps, 1, GUI_MAX_STEPS);
}

void GUISequencesPanel::draw_sequence() {
    if (current_sequence.pattern.notes.size() < static_cast<size_t>(current_sequence.pattern.steps)) {
        current_sequence.pattern.notes.resize(current_sequence.pattern.steps, UINT8_MAX);
    } else if (current_sequence.pattern.notes.size() > static_cast<size_t>(current_sequence.pattern.steps)) {
        current_sequence.pattern.notes.resize(current_sequence.pattern.steps);
    }

    ImGui::Text("Pattern:");

    if (!is_index_valid()) {
        ImGui::Text("No sequence available.");
        ImGui::Columns(1);
        return;
    }

    draw_sequence_length();
    draw_pattern(current_sequence.pattern);
}

void GUISequencesPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    current_sequence.pattern.handle_input();
}
