#include <algorithm>

#include "../../general.hpp"
#include "../../maps/keys.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "../patterns/selection.hpp"
#include "sequences.hpp"

GUISequencesPanel::GUISequencesPanel(const bool visible, const bool windowed)
    : GUIPanel("Sequences", visible, windowed) {
    initialize();
}

GUIElement GUISequencesPanel::get_element() const {
    return GUIElement::Sequences;
}

void GUISequencesPanel::draw() {
    draw_sequence();
}

bool GUISequencesPanel::select_item() {
    std::vector<std::string> dependencies = song.find_sequence_dependencies(sequence_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    prepare_combo(this, sequence_names, "##SequenceCombo", sequence_index);
    show_dependency_tooltip(dependencies);
    pop_tertiary_style();
    ImGui::Separator();

    return !sequences.empty();
}

void GUISequencesPanel::empty() {
    ImGui::Text("No sequence available.");
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
    if (!save &&
        (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
         !is_index_valid())) {
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

void GUISequencesPanel::shortcut_actions() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    switch (selection_action) {
    case PatternSelectionAction::TransposeUp: {
        transpose_selected_rows(1);
        break;
    }
    case PatternSelectionAction::TransposeDown: {
        transpose_selected_rows(-1);
        break;
    }
    case PatternSelectionAction::TransposeOctaveUp: {
        transpose_selected_rows(scale_composer.get_edo());
        break;
    }
    case PatternSelectionAction::TransposeOctaveDown: {
        transpose_selected_rows(-scale_composer.get_edo());
        break;
    }
    case PatternSelectionAction::SelectAll:
    case PatternSelectionAction::SelectChannel: {
        select_all();
        break;
    }
    case PatternSelectionAction::DeselectAll: {
        deselect_all();
        break;
    }
    case PatternSelectionAction::Delete: {
        delete_selection();
        break;
    }
    case PatternSelectionAction::SetNoteRest: {
        set_selection_note(NOTE_REST);
        break;
    }
    case PatternSelectionAction::SetNoteCut: {
        set_selection_note(NOTE_CUT);
        break;
    }
    case PatternSelectionAction::SetNoteOff: {
        set_selection_note(NOTE_OFF);
        break;
    }
    case PatternSelectionAction::None:
    default: {
        break;
    }
    }

    selection_action = PatternSelectionAction::None;
}

void GUISequencesPanel::post_actions() {
    selection_action = PatternSelectionAction::None;
}

void GUISequencesPanel::select_all() {
    selection.select(0, current_sequence.pattern.steps - 1);
}

void GUISequencesPanel::deselect_all() {
    selection.clear();
}

void GUISequencesPanel::set_selection_note(const uint8_t note) {
    if (selection.is_active()) {
        for (size_t row = selection.start; row <= selection.end; row++) {
            const uint8_t row_note = row == selection.start ? note : NOTE_REST;
            current_sequence.pattern.set_note(row, row_note);
        }
    } else {
        current_sequence.pattern.set_note(current_sequence.pattern.current_row, note);
    }
}

void GUISequencesPanel::delete_selection() {
    if (selection.is_active()) {
        for (size_t row = selection.start; row <= selection.end; row++) {
            current_sequence.pattern.clear_row(row);
        }
    } else {
        const int row = current_sequence.pattern.current_row;
        current_sequence.pattern.clear_row(row);
    }
}

void GUISequencesPanel::transpose_selected_rows(const int value) {
    if (value == 0 || !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (selection.is_active()) {
        for (size_t i = selection.start; i <= selection.end; i++) {
            current_sequence.pattern.transpose(value, i);
        }
    } else {
        current_sequence.pattern.transpose(value);
    }
}

void GUISequencesPanel::draw_sequence_length() {
    const size_t old_size = current_sequence.pattern.steps;
    const LinkKey key = {Target::SPECIAL, sequence_index, SPECIAL_SEQUENCE_LENGTH};
    draw_number_of_items(this, "Steps", "##SequenceLength", current_sequence.pattern.steps, 1, MAX_STEPS, key);

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
    PatternSelection empty_selection;
    PatternSelection &sequence_selection = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ? selection : empty_selection;
    SequenceRows secondary_sequence_rows;

    draw_sequence_length();
    draw_pattern(current_sequence.pattern, sequence_selection, secondary_sequence_rows, false);
}

void GUISequencesPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    const int old_row = current_sequence.pattern.current_row;
    const uint8_t old_note = current_sequence.pattern.is_row_valid(current_sequence.pattern.current_row) ? current_sequence.pattern.notes[old_row] : NOTES;
    current_sequence.pattern.handle_input();
    if (old_note != NOTES) {
        const uint8_t new_note = current_sequence.pattern.notes[old_row];
        const PatternRow pattern_row = {0, 0, old_row};
        const uint16_t offset = SEQUENCE_NOTES + sizeof(Note) * old_row;
        const LinkKey key = {Target::SEQUENCE, sequence_index, offset};
        perform_action_note(this, key, pattern_row, old_note, new_note);
    }
}

void GUISequencesPanel::set_note(const size_t channel_index, const int row, const uint8_t note) {
    current_sequence.pattern.set_note(row, note);
    current_sequence.pattern.current_row = row;
}

void GUISequencesPanel::set_index(const int index) {
    sequence_index = clamp_index(index, sequences.size());
}

bool GUISequencesPanel::is_active() const {
    return visible && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
}

void GUISequencesPanel::register_shortcuts() {
    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeUp,
        [this]() {
            selection_action = PatternSelectionAction::TransposeUp;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeDown,
        [this]() {
            selection_action = PatternSelectionAction::TransposeDown;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeOctaveUp,
        [this]() {
            selection_action = PatternSelectionAction::TransposeOctaveUp;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeOctaveDown,
        [this]() {
            selection_action = PatternSelectionAction::TransposeOctaveDown;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectAll,
        [this]() {
            selection_action = PatternSelectionAction::SelectAll;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectNone,
        [this]() {
            selection_action = PatternSelectionAction::DeselectAll;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditDelete,
        [this]() {
            selection_action = PatternSelectionAction::Delete;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSetNoteRest,
        [this]() {
            selection_action = PatternSelectionAction::SetNoteRest;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSetNoteCut,
        [this]() {
            selection_action = PatternSelectionAction::SetNoteCut;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSetNoteOff,
        [this]() {
            selection_action = PatternSelectionAction::SetNoteOff;
        }
    );
}
