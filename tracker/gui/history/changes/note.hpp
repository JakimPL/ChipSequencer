#pragma once

#include <cstddef>
#include <cstdint>

struct NoteChange {
    size_t channel_index;
    size_t pattern_id;
    size_t note_index;
    uint8_t old_note;
    uint8_t new_note;
};
