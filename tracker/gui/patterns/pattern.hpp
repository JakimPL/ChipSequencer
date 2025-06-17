#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../structures/sequence.hpp"

struct Pattern {
    Pattern();
    Pattern(const uint8_t sequence_index);

    int starting_row = 0;
    int current_row = -1;
    uint8_t sequence_index = -1;
    int steps = 0;
    std::vector<uint8_t> notes = {};
    std::vector<uint8_t> durations = {};
    std::vector<uint8_t> indices = {};

    void from_sequence(const uint8_t sequence_index);
    std::vector<Note> to_note_vector() const;
    void to_buffer(const size_t sequence_index) const;
    int calculate_playing_row(size_t channel_index);

    void clear_row(const int row);
    bool is_row_valid(const int row) const;
    void jump(const int max_row = -1);
    void insert_note(const int note_index, const int edo, const int max_row = -1);
    int get_note(const int row) const;
    void set_note(const int row, const int note);
    void transpose(const int value, std::optional<int> row = std::nullopt);
    void handle_input(const int min_row = 0, const int max_row = -1);
};
