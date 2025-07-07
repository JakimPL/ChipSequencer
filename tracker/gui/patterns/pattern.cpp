#include <algorithm>

#include "../../constants.hpp"
#include "../../general.hpp"
#include "../../maps/keys.hpp"
#include "../../song/buffers.hpp"
#include "../../song/core.hpp"
#include "../../tuning/frequencies.hpp"
#include "../../tuning/scale.hpp"
#include "../../utils/math.hpp"
#include "../gui.hpp"
#include "../shortcuts/manager.hpp"
#include "pattern.hpp"

Pattern::Pattern(const uint8_t sequence_index) {
    from_sequence(sequence_index);
}

void Pattern::from_sequence(const uint8_t index) {
    sequence_index = index;
    const Sequence *sequence = sequences[sequence_index];
    uint16_t total_length = 0;

    notes.clear();
    indices.clear();
    durations.clear();
    for (size_t i = 0; i < sequence->size / sizeof(Note); ++i) {
        uint16_t duration = sequence->notes[i].duration;
        durations.push_back(duration);
        indices.push_back(total_length);
        total_length += duration;
        notes.push_back(sequence->notes[i].pitch);
        for (uint16_t j = 1; j < duration; ++j) {
            notes.push_back(NOTE_REST);
        }
    }

    steps = total_length;
}

void Pattern::to_buffer(const size_t sequence_index) const {
    if (sequence_index == -1) {
        return;
    }

    const Sequence *sequence = sequences[sequence_index];
    const size_t sequence_length = sequence->size / sizeof(Note);
    for (size_t i = 0; i < sequence_length; ++i) {
        const uint8_t pitch = notes[i];
        if (pitch == NOTE_OFF) {
            continue;
        }
        buffers.sequences[sequence_index][i] = pitch;
    }
}

std::vector<Note> Pattern::to_note_vector() const {
    std::vector<Note> note_vector;

    uint8_t duration = 1;
    uint8_t pitch;
    for (int i = steps - 1; i >= 0; --i) {
        if (i >= notes.size()) {
            if (note_vector.empty()) {
                note_vector.push_back({NOTE_REST, 1});
            }
            continue;
        }

        pitch = notes[i];
        if (pitch == NOTE_REST && i > 0) {
            ++duration;
        } else {
            Note note = {pitch, duration};
            note_vector.push_back(note);
            duration = 1;
        }
    }

    std::reverse(note_vector.begin(), note_vector.end());
    return note_vector;
}

int Pattern::calculate_playing_row(size_t channel_index) {
    if (indices.empty()) {
        return -1;
    }

    size_t note_index = sequence_current_note.at(channel_index) - 1;
    note_index = std::min(note_index, indices.size() - 1);
    try {
        int playing_row = indices.at(note_index);
        playing_row += durations.at(note_index);
        playing_row -= sequence_timer_row.at(channel_index);
        return playing_row;
    } catch (const std::out_of_range &) {
        return -1;
    }
}

void Pattern::clear_row(const int row) {
    if (!is_row_valid(row)) {
        return;
    }

    notes[row] = NOTE_REST;
}

bool Pattern::is_row_valid(const int row) const {
    return row >= 0 && row < static_cast<int>(notes.size());
}

void Pattern::jump(const int max_row) {
    const int max = max_row == -1 ? steps : std::min(steps, max_row);
    current_row = std::min(current_row + gui.get_jump_step(), max - 1);
}

void Pattern::insert_note(const int note_index, const int edo, const int max_row) {
    const int a4_index = frequency_table.get_a4_index();
    const int note = note_index + a4_index + edo * (gui.get_current_octave() - 5);
    set_note(current_row, note);
    jump(max_row);
}

int Pattern::get_note(const int row) const {
    if (!is_row_valid(row)) {
        return NOTES;
    }

    return notes[row];
}

void Pattern::set_note(const int row, const int note) {
    if (!is_row_valid(row)) {
        return;
    }

    if (note < 0 || (note >= NOTES && note < NOTE_CUT)) {
        return;
    }

    notes[row] = static_cast<uint8_t>(note);
}

void Pattern::transpose(const int value, std::optional<int> row) {
    if (!row.has_value()) {
        row = current_row;
    }

    if (!is_row_valid(row.value())) {
        return;
    }

    if (notes[row.value()] >= NOTES) {
        return;
    }

    notes[row.value()] = clamp(notes[row.value()] + value, 0, NOTES - 1);
}

void Pattern::handle_input(const int min_row, const int max_row) {
    if (!is_row_valid(current_row)) {
        return;
    }

    const int edo = scale_composer.get_edo();
    if (!ImGui::GetIO().KeyCtrl &&
        !ImGui::GetIO().KeyShift &&
        !ImGui::GetIO().KeyAlt) {
        if (edo == DEFAULT_EDO) {
            for (const auto &m : key_note_12_edo_mapping) {
                if (ImGui::IsKeyPressed(m.key)) {
                    insert_note(m.note_index, edo, max_row);
                    break;
                }
            }
        } else {
            for (const auto &m : key_note_linear_mapping) {
                if (ImGui::IsKeyPressed(m.key)) {
                    insert_note(m.note_index, edo, max_row);
                    break;
                }
            }
        }
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::SequenceNoteDelete)) {
        notes[current_row] = NOTE_REST;
        jump(max_row);
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::SequenceSetNoteOff)) {
        notes[current_row] = NOTE_OFF;
        jump(max_row);
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::SequenceSetNoteCut)) {
        notes[current_row] = NOTE_CUT;
        jump(max_row);
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::SequenceSetNoteRest)) {
        notes[current_row] = NOTE_REST;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        const int min = std::max(0, min_row);
        current_row = std::max(min, current_row - 1);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        const int max = max_row == -1 ? steps : std::min(steps, max_row);
        current_row = std::min(max - 1, current_row + 1);
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditIncrement)) {
        steps = std::min(steps, MAX_STEPS);
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditDecrement)) {
        steps = std::max(steps, 1);
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditHome)) {
        current_row = 0;
    }

    if (shortcut_manager.is_shortcut_pressed(ShortcutAction::EditEnd)) {
        const int max = max_row == -1 ? steps : std::min(steps, max_row);
        current_row = max - 1;
    }
}
