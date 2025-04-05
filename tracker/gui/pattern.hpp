#ifndef GUI_PATTERN_HPP
#define GUI_PATTERN_HPP

#include <cstdint>
#include <vector>

#include "../structures/sequence.hpp"

struct Pattern {
    Pattern();
    Pattern(const uint8_t sequence_index);

    uint8_t sequence_index = -1;
    int current_row = -1;
    int steps = 0;
    std::vector<uint8_t> notes = {};
    std::vector<uint8_t> durations = {};
    std::vector<uint8_t> indices = {};

    void from_sequence(const uint8_t sequence_index);
    std::vector<Note> to_note_vector() const;
    int calculate_playing_row(size_t channel_index);

    void jump();
    void set_note(const int note_index, const int edo);
    void handle_input();

    static Sequence *to_sequence(const std::vector<Note> &note_vector);
};

#endif // GUI_PATTERN_HPP
