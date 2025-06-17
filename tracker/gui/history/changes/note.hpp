#pragma once

#include <cstddef>
#include <cstdint>

struct NoteChange {
    size_t channel_index;
    size_t pattern_id;
    int row;
    uint8_t old_note;
    uint8_t new_note;
};
