#include <algorithm>

#include "../../general.hpp"
#include "../../maps/keys.hpp"
#include "../../song/buffers.hpp"
#include "../../song/song.hpp"
#include "../../song/lock/registry.hpp"
#include "../../utils/math.hpp"
#include "../enums.hpp"
#include "../gui.hpp"
#include "../init.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "../clipboard/clipboard.hpp"
#include "../clipboard/items/notes.hpp"
#include "../patterns/selection.hpp"
#include "sequences.hpp"

GUISequencesPanel::GUISequencesPanel(const bool visible, const bool windowed)
    : GUIPanel("Sequences", visible, windowed) {
}

GUIElement GUISequencesPanel::get_element() const {
    return GUIElement::Sequences;
}

void GUISequencesPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::SEQUENCE, sequence_index));
    draw_sequence();
    ImGui::EndDisabled();
}

bool GUISequencesPanel::select_item() {
    std::vector<std::string> dependencies = song.find_sequence_dependencies(sequence_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    prepare_combo(this, sequence_names, "##SequenceCombo", sequence_index, {}, false, GUI_COMBO_MARGIN_RIGHT);
    show_dependency_tooltip(dependencies);
    lock_item(Target::SEQUENCE, sequence_index);
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
    const bool focus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    if (!is_index_valid()) {
        return;
    }

    if (!save && !focus && !dialog_box_open) {
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
    perform_action_add(this, {Target::SEQUENCE, sequence_index, 0});
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
        perform_action_remove(this, {Target::SEQUENCE, sequence_index, 0}, sequences[sequence_index]);
        song.remove_sequence(sequence_index);
        sequence_index = std::max(0, sequence_index - 1);
        update();
    }
}

void GUISequencesPanel::update() {
    sequence_index = clamp_index(sequence_index, sequences.size());
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
    case PatternSelectionAction::Cut: {
        copy_selection();
        delete_selection();
        break;
    }
    case PatternSelectionAction::Copy: {
        copy_selection();
        break;
    }
    case PatternSelectionAction::Paste: {
        paste_selection();
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
}

void GUISequencesPanel::post_actions() {
    dialog_box_open = edit_dialog_box.visible;
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
        PatternSelectionChange<uint8_t> changes;
        for (int row = selection.start; row <= selection.end; row++) {
            const uint8_t old_note = current_sequence.pattern.get_note(row);
            current_sequence.pattern.clear_row(row);
            const uint8_t new_note = current_sequence.pattern.get_note(row);
            changes[{0, 0, row}] = {old_note, new_note};
        }

        perform_notes_action("Delete", changes);
    } else {
        const int row = current_sequence.pattern.current_row;
        const uint8_t old_note = current_sequence.pattern.get_note(row);
        current_sequence.pattern.clear_row(row);
        const uint8_t new_note = current_sequence.pattern.get_note(row);
        perform_note_action(row, old_note, new_note);
    }
}

void GUISequencesPanel::copy_selection() {
    const int start = selection.is_active() ? selection.start : current_sequence.pattern.current_row;
    const int end = selection.is_active() ? selection.end : start;

    std::vector<uint8_t> notes;
    PatternNotes pattern_notes;

    for (int row = start; row <= end; ++row) {
        const uint8_t note = current_sequence.pattern.get_note(row);
        notes.push_back(note);
    }

    pattern_notes.push_back(notes);
    clipboard.add_item(
        std::make_unique<ClipboardNotes>(pattern_notes)
    );
}

void GUISequencesPanel::paste_selection() {
    ClipboardItem *item = clipboard.get_recent_item(ClipboardCategory::Notes);
    ClipboardNotes *notes = dynamic_cast<ClipboardNotes *>(item);
    if (!notes) {
        return;
    }

    const PatternNotes &pattern_notes = notes->pattern_notes;
    if (pattern_notes.empty()) {
        return;
    }

    PatternSelectionChange<uint8_t> changes;
    const int current_row = current_sequence.pattern.current_row;
    for (size_t i = 0; i < pattern_notes[0].size(); ++i) {
        const int row = current_row + i;
        if (row >= current_sequence.pattern.notes.size()) {
            break;
        }

        const PatternRow pattern_row = {0, 0, row};
        const uint8_t old_note = current_sequence.pattern.notes[row];
        const uint8_t note = pattern_notes[0][i];

        set_note(row, note);
        changes[pattern_row] = {old_note, note};
    }

    perform_notes_action("Paste", changes);
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

void GUISequencesPanel::perform_notes_action(const std::string &action_name, const PatternSelectionChange<uint8_t> &changes) {
    SetItemsFunction<uint8_t> function = [this](const std::map<PatternRow, uint8_t> &notes) {
        this->set_notes(notes);
    };

    perform_action_pattern_selection<uint8_t>(this, {Target::SEQUENCE}, action_name, changes, function);
}

void GUISequencesPanel::perform_note_action(const int row, const uint8_t old_note, const uint8_t new_note) {
    const PatternRow pattern_row = {0, 0, row};
    const uint16_t offset = SEQUENCE_NOTES + sizeof(Note) * row;
    const LinkKey key = {Target::SEQUENCE, sequence_index, offset};
    perform_action_note(this, key, pattern_row, old_note, new_note);
}

void GUISequencesPanel::open_edit_dialog_box(const int item) {
    if (item < 0 || item >= current_sequence.pattern.steps) {
        return;
    }

    dialog_box_open = true;
    edit_dialog_box.visible = true;
    edit_dialog_box.item = item;
    edit_dialog_box.note = mod(current_sequence.pattern.get_note(item) - NOTE_CUT, 256);
}

void GUISequencesPanel::draw_dialog_box() {
    if (!edit_dialog_box.visible) {
        return;
    }

    if (!ImGui::IsPopupOpen("Edit note")) {
        ImGui::OpenPopup("Edit note");
    }

    ImGui::SetNextWindowSize(ImVec2(450.0f, 100.0f), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal("Edit note", &edit_dialog_box.visible, ImGuiWindowFlags_NoCollapse)) {
        std::vector<std::string> note_names = get_note_names();

        ImGui::Text("Note:");
        push_tertiary_style();
        prepare_combo(this, note_names, "##EditNote", edit_dialog_box.note);
        pop_tertiary_style();
        ImGui::Separator();

        GUIAction action = draw_dialog_box_bottom();
        switch (action) {
        case GUIAction::OK: {
            const uint8_t old_note = current_sequence.pattern.get_note(edit_dialog_box.item);
            const uint8_t note = mod(edit_dialog_box.note + NOTE_CUT, 256);
            set_note(edit_dialog_box.item, note);
            perform_note_action(edit_dialog_box.item, old_note, note);
        }
        case GUIAction::Cancel: {
            edit_dialog_box.visible = false;
            break;
        }
        case GUIAction::None:
        default: {
            break;
        }
        }

        ImGui::EndPopup();
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

    draw_sequence_length();
    if (current_sequence.pattern.steps > 0) {
        if (ImGui::Button("Edit")) {
            open_edit_dialog_box(current_sequence.pattern.current_row);
        }
    }

    SequenceRows secondary_sequence_rows;
    const int edited_row = edit_dialog_box.visible ? edit_dialog_box.item : -1;
    draw_pattern(current_sequence.pattern, sequence_selection, secondary_sequence_rows, false, edited_row);
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
        const uint8_t new_note = current_sequence.pattern.get_note(old_row);
        perform_note_action(old_row, old_note, new_note);
    }
}

void GUISequencesPanel::set_notes(const std::map<PatternRow, uint8_t> &notes) {
    for (const auto &[pattern_row, note] : notes) {
        set_note(pattern_row, note);
    }
}

void GUISequencesPanel::set_note(const PatternRow &pattern_row, const uint8_t note) {
    set_note(pattern_row.row, note);
}

void GUISequencesPanel::set_note(const int row, const uint8_t note) {
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
        ShortcutAction::EditCut,
        [this]() {
            selection_action = PatternSelectionAction::Cut;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditCopy,
        [this]() {
            selection_action = PatternSelectionAction::Copy;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditPaste,
        [this]() {
            selection_action = PatternSelectionAction::Paste;
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
