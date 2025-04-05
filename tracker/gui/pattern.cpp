#include <algorithm>

#include "../constants.hpp"
#include "../general.hpp"
#include "../song/data.hpp"
#include "mapping.hpp"
#include "pattern.hpp"

Pattern::Pattern() {
}

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
    for (size_t i = 0; i < sequence->data_size / 2; ++i) {
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
    size_t note_index = sequence_current_note[channel_index] - 1;
    int playing_row = indices[note_index];
    playing_row += durations[note_index];
    playing_row -= 1 + sequence_timer[channel_index] / ticks_per_beat;
    return playing_row;
}

void Pattern::jump() {
    current_row = std::min(current_row + gui.get_jump_step(), steps - 1);
}

void Pattern::set_note(const int note_index, const int edo) {
    const int a4_index = frequency_table.get_a4_index();
    const int note = note_index + a4_index + edo * (gui.get_current_octave() - 5);
    if (note < 0 || note >= NOTES) {
        return;
    }
    notes[current_row] = note;
    jump();
}

void Pattern::handle_input() {
    const bool valid = current_row >= 0 && current_row < notes.size();
    if (!valid) {
        return;
    }

    const int edo = scale_composer.get_edo();
    if (edo == DEFAULT_EDO) {
        for (const auto &m : key_note_12_edo_mapping) {
            if (ImGui::IsKeyPressed(m.key)) {
                set_note(m.note_index, edo);
                break;
            }
        }
    } else {
        for (const auto &m : key_note_linear_mapping) {
            if (ImGui::IsKeyPressed(m.key)) {
                set_note(m.note_index, edo);
                break;
            }
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Space)) {
        notes[current_row] = NOTE_REST;
        jump();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Apostrophe) || ImGui::IsKeyPressed(ImGuiKey_Equal)) {
        notes[current_row] = NOTE_OFF;
        jump();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        notes[current_row] = NOTE_REST;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        current_row = std::max(0, current_row - 1);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        current_row = std::min(steps - 1, current_row + 1);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd)) {
        steps = std::min(steps, GUI_MAX_STEPS);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract)) {
        steps = std::max(steps, 1);
    }
}

Sequence *Pattern::to_sequence(const std::vector<Note> &note_vector) {
    const size_t length = note_vector.size();
    const uint8_t data_size = static_cast<uint8_t>(length * sizeof(Note));
    const size_t structure_size = data_size + 1;

    Sequence *new_sequence = static_cast<Sequence *>(operator new(structure_size));
    new_sequence->data_size = data_size;
    std::copy(note_vector.begin(), note_vector.end(), new_sequence->notes);
    return new_sequence;
}
