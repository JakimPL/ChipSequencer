#include <algorithm>

#include "../constants.hpp"
#include "pattern.hpp"

void Pattern::from_sequence(Sequence *sequence) {
    uint16_t total_length = 0;

    notes.clear();
    for (size_t i = 0; i < sequence->data_size / 2; ++i) {
        uint16_t duration = sequence->notes[i].duration;
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