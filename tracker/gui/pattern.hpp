#ifndef GUI_PATTERN_HPP
#define GUI_PATTERN_HPP

#include <cstdint>
#include <vector>

#include "../structures/sequence.hpp"

struct Pattern {
    Pattern();
    Pattern(const Sequence *sequence);

    int current_row = -1;
    int steps = 0;
    std::vector<uint8_t> notes = {};
    std::vector<uint8_t> durations = {};
    std::vector<uint8_t> indices = {};

    void from_sequence(const Sequence *sequence);
    std::vector<Note> to_note_vector() const;
    int calculate_playing_row(size_t channel_index);
};

#endif // GUI_PATTERN_HPP
