#include <algorithm>

#include "../../general.hpp"
#include "../../maps/keys.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "../patterns/selection.hpp"
#include "sequences.hpp"

GUISequencesPanel::GUISequencesPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeUp,
        [this]() {
            transpose_by = 1;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeDown,
        [this]() {
            transpose_by = -1;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeOctaveUp,
        [this]() {
            transpose_by = scale_composer.get_edo();
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeOctaveDown,
        [this]() {
            transpose_by = -scale_composer.get_edo();
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectAll,
        [this]() {
            selection_mode = PatternSelectionMode::All;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectNone,
        [this]() {
            selection_mode = PatternSelectionMode::None;
        }
    );
}

void GUISequencesPanel::draw() {
    ImGui::Begin("Sequences");
    ImGui::Columns(1, "sequence_columns");

    std::vector<std::string> dependencies = song.find_sequence_dependencies(sequence_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    prepare_combo(sequence_names, "##SequenceCombo", sequence_index);
    show_dependency_tooltip(dependencies);
    pop_tertiary_style();

    ImGui::Separator();

    from();
    draw_sequence();
    select();
    transpose_selected_rows();
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
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || !is_index_valid()) {
        return;
    }

    Sequence *sequence = sequences[sequence_index];
    current_sequence.pattern.to_buffer(sequence_index);
    const std::vector<Note> note_vector = current_sequence.pattern.to_note_vector();
    sequence->from_note_vector(note_vector);
}

void GUISequencesPanel::add() {
    Sequence *new_sequence = song.add_sequence();
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = sequences.size() - 1;
    update();
}

void GUISequencesPanel::duplicate() {
    Sequence *new_sequence = song.duplicate_sequence(sequence_index);
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

void GUISequencesPanel::select() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    switch (selection_mode) {
    case PatternSelectionMode::All:
    case PatternSelectionMode::Channel: {
        select_all();
        break;
    }
    case PatternSelectionMode::None: {
        deselect_all();
        break;
    }
    case PatternSelectionMode::Ignore:
    default: {
        break;
    }
    }

    selection_mode = PatternSelectionMode::Ignore;
}

void GUISequencesPanel::select_all() {
    selection.select(0, current_sequence.pattern.steps - 1);
}

void GUISequencesPanel::deselect_all() {
    selection.clear();
}

void GUISequencesPanel::transpose_selected_rows() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        transpose_by = 0;
    }

    if (transpose_by == 0) {
        return;
    }

    if (selection.is_active()) {
        for (size_t i = selection.start; i <= selection.end; i++) {
            current_sequence.pattern.transpose(transpose_by, i);
        }
    } else {
        current_sequence.pattern.transpose(transpose_by);
    }

    transpose_by = 0;
}

void GUISequencesPanel::draw_sequence_length() {
    const size_t old_size = current_sequence.pattern.steps;
    draw_number_of_items("Steps", "##SequenceLength", current_sequence.pattern.steps, 1, MAX_STEPS);

    if (old_size != current_sequence.pattern.steps) {
        current_sequence.pattern.notes.resize(current_sequence.pattern.steps);
        if (current_sequence.pattern.steps > old_size) {
            for (size_t i = old_size; i < current_sequence.pattern.steps; i++) {
                current_sequence.pattern.notes[i] = buffers.sequences[sequence_index][i];
            }
        }
    }
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

    PatternSelection empty_selection;
    PatternSelection &sequence_selection = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ? selection : empty_selection;
    PatternRows secondary_pattern_rows;

    draw_sequence_length();
    draw_pattern(current_sequence.pattern, sequence_selection, secondary_pattern_rows, false);
}

void GUISequencesPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    current_sequence.pattern.handle_input();
}

void GUISequencesPanel::set_index(const int index) {
    sequence_index = clamp_index(index, sequences.size());
}
