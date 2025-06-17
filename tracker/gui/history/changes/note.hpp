#pragma once

#include <cstddef>
#include <cstdint>

#include "../../patterns/selection.hpp"

struct NoteChange {
    PatternRow pattern_row;
    uint8_t old_note;
    uint8_t new_note;
};
