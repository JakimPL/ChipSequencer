#ifndef GUI_PATTERN_HPP
#define GUI_PATTERN_HPP

#include <cstdint>
#include <vector>

#include "../structures/sequence.hpp"

struct Pattern {
    int current_row = -1;
    int steps = 0;
    std::vector<uint8_t> notes = {};

    void from_sequence(Sequence *sequence);
    std::vector<Note> to_note_vector() const;
};

#endif // GUI_PATTERN_HPP
